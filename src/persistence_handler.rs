use futures::TryFutureExt;
use std::thread;
use tokio::fs::{remove_file, File};
use tokio::io::{AsyncWriteExt, Result};
use tokio::sync::mpsc::Receiver;
use tokio::{runtime::Runtime, task};

use crate::game_object::GameObjectRef;

#[derive(Debug)]
pub enum PersistenceRequest {
    PersistObject(GameObjectRef, String),
    RemoveObject(GameObjectRef),
}

pub fn create_persistence_handler(data_dir: String, mut persist_rx: Receiver<PersistenceRequest>) {
    thread::spawn(move || {
        let rt = Runtime::new().unwrap();
        let local = task::LocalSet::new();
        local.block_on(&rt, async move {
            task::spawn_local(async move {
                while let Some(peristence_req) = persist_rx.recv().await {
                    match peristence_req {
                        PersistenceRequest::PersistObject(object_ref, data) => {
                            let result =
                                File::create(format!("{}/{}", data_dir, object_ref.to_file_name()))
                                    .and_then(|file| write_all(file, data))
                                    .await;
                            if let Err(error) = result {
                                println!("Cannot persist object {:?}: {:?}", object_ref, error);
                            }
                        }

                        PersistenceRequest::RemoveObject(object_ref) => {
                            if let Err(error) =
                                remove_file(format!("{}/{}", data_dir, object_ref.to_file_name()))
                                    .await
                            {
                                println!("Cannot remove object {:?}: {:?}", object_ref, error);
                            }
                        }
                    }
                }
            })
            .await
            .unwrap();
        });
    });
}

async fn write_all(mut file: File, data: String) -> Result<()> {
    file.write_all(data.as_bytes()).await
}
