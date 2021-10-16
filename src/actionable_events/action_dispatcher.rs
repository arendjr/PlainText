use super::ActionableEvent;
use futures::future::{AbortHandle, Abortable};
use std::time::Duration;
use tokio::{sync::mpsc::Sender, time::sleep};

pub struct ActionDispatcher {
    action_tx: Sender<ActionableEvent>,
}

impl ActionDispatcher {
    pub fn new(action_tx: Sender<ActionableEvent>) -> Self {
        Self { action_tx }
    }

    pub fn dispatch_after(&self, action: ActionableEvent, delay: Duration) {
        tokio::spawn(dispatch_after(self.action_tx.clone(), action, delay));
    }

    pub fn dispatch_cancelable_after(
        &self,
        action: ActionableEvent,
        delay: Duration,
    ) -> AbortHandle {
        let (abort_handle, abort_registration) = AbortHandle::new_pair();
        tokio::spawn(Abortable::new(
            dispatch_after(self.action_tx.clone(), action, delay),
            abort_registration,
        ));
        abort_handle
    }
}

async fn dispatch_after(sender: Sender<ActionableEvent>, action: ActionableEvent, delay: Duration) {
    sleep(delay).await;

    sender
        .send(action)
        .await
        .expect("Failed to dispatch action");
}
