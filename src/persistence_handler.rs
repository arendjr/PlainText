use crate::entity::EntityRef;
use futures::TryFutureExt;
use std::thread;
use tokio::{
    fs::{remove_file, File},
    io::{AsyncWriteExt, Result},
    runtime::Runtime,
    sync::mpsc::Receiver,
    task,
};

#[derive(Debug)]
pub enum PersistenceRequest {
    PersistEntity(EntityRef, String),
    RemoveEntity(EntityRef),
}

pub fn create_persistence_handler(data_dir: String, mut persist_rx: Receiver<PersistenceRequest>) {
    thread::spawn(move || {
        let rt = Runtime::new().unwrap();
        let local = task::LocalSet::new();
        local.block_on(&rt, async move {
            task::spawn_local(async move {
                while let Some(peristence_req) = persist_rx.recv().await {
                    match peristence_req {
                        PersistenceRequest::PersistEntity(entity_ref, data) => {
                            let result =
                                File::create(format!("{}/{}", data_dir, entity_ref.to_file_name()))
                                    .and_then(|file| write_all(file, data))
                                    .await;
                            if let Err(error) = result {
                                println!("Cannot persist entity {:?}: {:?}", entity_ref, error);
                            }
                        }

                        PersistenceRequest::RemoveEntity(entity_ref) => {
                            if let Err(error) =
                                remove_file(format!("{}/{}", data_dir, entity_ref.to_file_name()))
                                    .await
                            {
                                println!("Cannot remove entity {:?}: {:?}", entity_ref, error);
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
