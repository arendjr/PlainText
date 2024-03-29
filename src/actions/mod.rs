use crate::player_output::PlayerOutput;

mod change_direction_action;
mod die_action;
mod disband_action;
mod enter_action;
mod follow_action;
mod kill_action;
mod look_action;
mod lose_action;
mod open_action;
mod set_character_action_action;
mod shout_action;
mod stun_action;

pub use change_direction_action::change_direction;
pub use die_action::die;
pub use disband_action::disband;
pub use enter_action::{enter_current_room, enter_portal, enter_room, leave_room};
pub use follow_action::{follow, unfollow};
pub use kill_action::kill;
pub use look_action::{look_at_entity, look_in_direction};
pub use lose_action::lose;
pub use open_action::{close, open};
pub use set_character_action_action::set_character_action;
pub use shout_action::shout;
pub use stun_action::stun;

pub type ActionOutput = Result<Vec<PlayerOutput>, String>;
