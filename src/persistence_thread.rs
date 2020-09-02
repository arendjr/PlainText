use std::fs::{remove_file, File};
use std::io::Write;
use std::sync::mpsc::Receiver;
use std::thread;

use crate::game_object::GameObjectRef;

pub enum PersistenceRequest {
    PersistObject(GameObjectRef, String),
    RemoveObject(GameObjectRef),
}

pub fn create_persistence_thread(data_dir: String, persist_rx: Receiver<PersistenceRequest>) {
    thread::spawn(move || {
        while let Ok(peristence_req) = persist_rx.recv() {
            match peristence_req {
                PersistenceRequest::PersistObject(object_ref, data) => {
                    match File::create(format!("{}/{}", data_dir, object_ref.to_file_name())) {
                        Ok(mut file) => {
                            file.write_all(data.as_bytes());
                        }

                        Err(error) => {
                            println!("Cannot persist object {:?}: {:?}", object_ref, error);
                        }
                    }
                }

                PersistenceRequest::RemoveObject(object_ref) => {
                    if let Err(error) =
                        remove_file(format!("{}/{}", data_dir, object_ref.to_file_name()))
                    {
                        println!("Cannot remove object {:?}: {:?}", object_ref, error);
                    }
                }
            }
        }
    });
}
