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
