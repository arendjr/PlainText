use chrono::{Local, NaiveDate};
use std::collections::HashMap;
use std::fs::{create_dir_all, File, OpenOptions};
use std::io;
use std::io::Write;
use std::path::Path;
use std::sync::mpsc::Receiver;
use std::thread;

use crate::log_messages::LogMessage;

pub fn create_log_thread(log_dir: String, log_rx: Receiver<Box<dyn LogMessage>>) {
    thread::spawn(move || {
        let mut day = Local::today().naive_local();
        let mut log_files: HashMap<String, File> = HashMap::new();

        while let Ok(log_message) = log_rx.recv() {
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
    });
}

fn create_log_file(log_dir: &String, log: &String, day: &NaiveDate) -> io::Result<File> {
    let dir_path = format!("{}/{}", log_dir, day.format("%Y%m%d"));
    create_dir_all(Path::new(&dir_path))?;

    let file_path = format!("{}/{}", dir_path, log);
    OpenOptions::new()
        .create(true)
        .append(true)
        .open(Path::new(&file_path))
}
