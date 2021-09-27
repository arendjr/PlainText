use async_trait::async_trait;
use core::fmt::Debug;
use tokio::io::Result;

use crate::game_object::GameObjectId;

use super::session_output::SessionOutput;
use super::sign_in::SignInState;

#[derive(Clone, Debug)]
pub enum SessionState {
    SessionClosed(Option<GameObjectId>),
    SigningIn(Box<SignInState>),
    SignedIn(GameObjectId),
}

#[async_trait]
pub trait Session: Debug + Send + Sync {
    async fn close(&mut self) -> Result<()>;
    async fn send(&mut self, output: SessionOutput);
    fn set_state(&mut self, state: SessionState);
    fn source(&self) -> String;
    fn state(&self) -> &SessionState;
}
