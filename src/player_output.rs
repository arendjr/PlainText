use crate::game_object::GameObjectId;
use crate::sessions::SessionOutput;

pub struct PlayerOutput {
    pub player_id: GameObjectId,
    pub output: SessionOutput,
}

impl PlayerOutput {
    pub fn new(player_id: GameObjectId, output: SessionOutput) -> Self {
        Self { player_id, output }
    }

    pub fn new_from_str(player_id: GameObjectId, output: &'static str) -> Self {
        Self::new(player_id, SessionOutput::Str(output))
    }

    pub fn new_from_string(player_id: GameObjectId, output: String) -> Self {
        Self::new(player_id, SessionOutput::String(output))
    }
}

#[macro_export]
macro_rules! push_output_str {
    ($player_output_vec:ident, $player_ref:ident, $session_output_str:expr) => {
        let session_output = crate::sessions::SessionOutput::Str($session_output_str);
        push_session_output!($player_output_vec, $player_ref, session_output);
    };
}

#[macro_export]
macro_rules! push_output_string {
    ($player_output_vec:ident, $player_ref:ident, $session_output_string:expr) => {
        let session_output = crate::sessions::SessionOutput::String($session_output_string);
        push_session_output!($player_output_vec, $player_ref, session_output);
    };
}

#[macro_export]
macro_rules! push_session_output {
    ($player_output_vec:ident, $player_ref:ident, $session_output:expr) => {
        let player_id = $player_ref.id();
        if let Some(player_output) = $player_output_vec
            .iter_mut()
            .find(|output| output.player_id == player_id)
        {
            let session_output = std::mem::replace(
                &mut player_output.output,
                crate::sessions::SessionOutput::None,
            );
            player_output.output = session_output.with($session_output);
        } else {
            $player_output_vec.push(PlayerOutput::new(player_id, $session_output));
        }
    };
}
