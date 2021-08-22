use chrono::{Local, NaiveDate};
use std::collections::HashMap;
use std::fs::{create_dir_all, File, OpenOptions};
use std::io;
use std::io::Write;
use std::path::Path;
use std::thread;
use tokio::{runtime::Runtime, sync::mpsc::Receiver, task};

use super::log_messages::LogMessage;

pub fn create_log_handler(log_dir: String, mut log_rx: Receiver<Box<dyn LogMessage>>) {
    thread::spawn(move || {
        let rt = Runtime::new().unwrap();
        let local = task::LocalSet::new();
        local.block_on(&rt, async move {
            task::spawn_local(async move {
                let mut day = Local::today().naive_local();
                let mut log_files: HashMap<String, File> = HashMap::new();

                while let Some(log_message) = log_rx.recv().await {
                    let now = Local::now().naive_local();
                    let today = now.date();
                    if day != today {
                        log_files.clear();
                        day = today;
                    }

                    let log = log_message.get_log();
                    let mut file = if let Some(file) = log_files.get(&log) {
                        file
                    } else {
                        match create_log_file(&log_dir, &log, &day) {
                            Ok(file) => {
                                log_files.insert(log.clone(), file);
                                log_files.get(&log).unwrap()
                            }
                            Err(error) => {
                                println!("Could not create log file: {:?}", error);
                                break;
                            }
                        }
                    };

                    let log_line = format!(
                        "{} {}\n",
                        now.format("%H:%M:%S%.3f"),
                        log_message.get_message()
                    );
                    if let Err(error) = file.write_all(log_line.as_bytes()) {
                        println!("Error writing log file: {:?}", error);
                        break;
                    }
                }

                println!("Logging disabled.");
            })
            .await
            .unwrap();
        });
    });
}

fn create_log_file(log_dir: &str, log: &str, day: &NaiveDate) -> io::Result<File> {
    let dir_path = format!("{}/{}", log_dir, day.format("%Y%m%d"));
    create_dir_all(Path::new(&dir_path))?;

    let file_path = format!("{}/{}", dir_path, log);
    OpenOptions::new().create(true).append(true).open(file_path)
}
