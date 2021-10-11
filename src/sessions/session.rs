use super::{session_output::SessionOutput, sign_in::SignInState};
use crate::entity::EntityId;
use async_trait::async_trait;
use core::fmt::Debug;
use tokio::io::Result;

#[derive(Clone, Debug)]
pub enum SessionState {
    SessionClosed(Option<EntityId>),
    SigningIn(Box<SignInState>),
    SignedIn(EntityId),
}

#[async_trait]
pub trait Session: Debug + Send + Sync {
    async fn close(&mut self) -> Result<()>;
    async fn send(&mut self, output: SessionOutput);
    fn set_state(&mut self, state: SessionState);
    fn source(&self) -> String;
    fn state(&self) -> &SessionState;
}
