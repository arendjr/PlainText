use std::io;
use std::io::Read;
use std::net::Shutdown;
use std::net::TcpStream;

const LF: u8 = 10;
const CR: u8 = 13;

const MAX_INPUT_LENGTH: usize = 1024;

pub struct SessionReader {
    input: [u8; 1024],
    input_length: usize,
    socket: TcpStream,
}

impl SessionReader {
    pub fn close(&mut self) -> io::Result<()> {
        self.socket.shutdown(Shutdown::Both)
    }

    pub fn new(socket: TcpStream) -> Self {
        Self {
            input: [0; MAX_INPUT_LENGTH],
            input_length: 0,
            socket,
        }
    }

    pub fn read_line(&mut self) -> Option<String> {
        while let Ok(num_bytes) = self.socket.read(&mut self.input[self.input_length..]) {
            if num_bytes == 0 {
                return None; // Must've reached EOL or someone is exceeding the buffer length.
            }

            if let Some(newline_index) = find_newline(self.input, self.input_length, num_bytes) {
                let result = String::from_utf8(self.input[0..newline_index].to_vec())
                    .ok()
                    .and_then(|string| Some(string.trim().to_owned()));

                // Rewind the input buffer for the next call:
                let input_length = self.input_length + num_bytes;
                let remaining_length = input_length - newline_index - 1;
                for i in 0..remaining_length {
                    self.input[i] = self.input[i + newline_index + 1];
                }
                self.input_length = 0;

                return result;
            } else {
                self.input_length += num_bytes;
            }
        }
        None
    }
}

fn find_newline(input: [u8; MAX_INPUT_LENGTH], start: usize, end: usize) -> Option<usize> {
    for i in start..end {
        if input[i] == LF || input[i] == CR {
            return Some(i);
        }
    }
    None
}
