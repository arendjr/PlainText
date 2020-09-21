use std::io;

use crate::game_object::GameObjectId;

use super::session_output::SessionOutput;
use super::sign_in::SignInState;

#[derive(Clone, Debug)]
pub enum SessionState {
    SessionClosed(Option<GameObjectId>),
    SigningIn(SignInState),
    SignedIn(GameObjectId),
}

pub trait Session: Send + Sync {
    fn close(&mut self) -> io::Result<()>;
    fn send(&mut self, output: SessionOutput);
    fn set_state(&mut self, state: SessionState);
    fn source(&self) -> String;
    fn state(&self) -> &SessionState;
}
