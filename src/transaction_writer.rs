use std::sync::Arc;
use tokio::sync::{mpsc, oneshot};

use crate::game_object::{GameObject, GameObjectMapReader, GameObjectMapWriter, GameObjectRef};

pub struct Transaction {
    pub updated_objects: Vec<Arc<dyn GameObject>>,
    pub deleted_objects: Vec<GameObjectRef>,
}

#[derive(Clone)]
pub struct TransactionWriter {
    channel: mpsc::Sender<(Transaction, oneshot::Sender<bool>)>,
}

impl TransactionWriter {
    pub async fn commit(&mut self, transaction: Transaction) -> bool {
        let (tx, rx) = oneshot::channel::<bool>();

        if let Err(error) = self.channel.send((transaction, tx)).await {
            panic!("Failed to send transaction: {}", error);
        }

        match rx.await {
            Ok(result) => result,
            Err(error) => panic!("Failed to receive transaction result: {}", error),
        }
    }

    pub fn new(_: GameObjectMapReader, mut object_map_writer: GameObjectMapWriter) -> Self {
        let (transaction_tx, mut transaction_rx) =
            mpsc::channel::<(Transaction, oneshot::Sender<bool>)>(100);
        tokio::spawn(async move {
            while let Some((transaction, sender)) = transaction_rx.recv().await {
                let result = if validate_transaction(&object_map_writer, &transaction) {
                    for object in transaction.updated_objects {
                        object_map_writer.set(object.get_ref(), object.clone())
                    }
                    for object_ref in transaction.deleted_objects {
                        object_map_writer.unset(object_ref);
                    }
                    object_map_writer.refresh();
                    sender.send(true)
                } else {
                    sender.send(false)
                };

                if let Err(error) = result {
                    panic!("Cannot send transaction result: {:?}", error);
                }
            }
        });

        Self {
            channel: transaction_tx,
        }
    }
}

fn validate_transaction(
    object_map_writer: &GameObjectMapWriter,
    transaction: &Transaction,
) -> bool {
    for object in &transaction.updated_objects {
        if !object_map_writer.can_set(object.get_ref(), object.clone()) {
            return false;
        }
    }

    for object_ref in &transaction.deleted_objects {
        if !object_map_writer.has(object_ref) {
            return false;
        }
    }

    true
}
