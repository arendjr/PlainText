use crate::game_object::GameObjectRef;
use crate::objects::Realm;
use crate::player_output::PlayerOutput;

mod go_command;

use go_command::go;

pub enum Command {
    Go(String),
}

pub fn execute_command(
    realm: Realm,
    player_ref: GameObjectRef,
    command: Command,
) -> (Realm, Vec<PlayerOutput>) {
    match command {
        Command::Go(where_to) => go(realm, player_ref, where_to),
    }
}
