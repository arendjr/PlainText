use std::collections::HashMap;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::Mutex;

use super::session::Session;

pub struct SessionManager {
    next_id: AtomicU64,
    session_map: Mutex<HashMap<u64, Session>>,
}

impl SessionManager {
    pub fn get_next_id(&self) -> u64 {
        self.next_id.fetch_add(1, Ordering::Relaxed)
    }

    pub fn new() -> Self {
        Self {
            next_id: AtomicU64::new(1),
            session_map: Mutex::new(HashMap::new()),
        }
    }

    pub fn register_session(&mut self, session: Session) {
        self.session_map
            .get_mut()
            .unwrap()
            .insert(session.id, session);
    }
}
