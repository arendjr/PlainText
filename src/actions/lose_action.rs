use crate::{
    actions,
    game_object::{GameObject, GameObjectRef},
    objects::Realm,
    player_output::PlayerOutput,
    text_utils::capitalize,
};

/// Cut a character loose from the group (only the leader should do this).
pub fn lose(realm: &mut Realm, follower_ref: GameObjectRef) -> Result<Vec<PlayerOutput>, String> {
    let follower = realm
        .character(follower_ref)
        .ok_or("Follower disappeared.")?;
    let group = follower
        .group()
        .and_then(|group| realm.group(group))
        .ok_or("They are no longer part of the group.")?;

    if follower_ref == group.leader() {
        Err("You cannot lose yourself from the group".into())
    } else if group.followers().contains(&follower_ref) {
        let group_ref = group.object_ref();
        if group.followers().len() > 1 {
            let leader = realm.character_res(group.leader())?;

            let output = vec![
                PlayerOutput::new_from_string(
                    follower.id(),
                    format!(
                        "{} has removed you from the group.\n",
                        capitalize(&leader.definite_name(realm)?)
                    ),
                ),
                PlayerOutput::new_from_string(
                    leader.id(),
                    format!(
                        "You removed {} from the group.\n",
                        follower.definite_name(realm)?
                    ),
                ),
            ];

            if let Some(group) = realm.group_mut(group_ref) {
                group.remove_follower(follower_ref);
            }

            Ok(output)
        } else {
            actions::disband(realm, group_ref)
        }
    } else {
        Err(format!(
            "{} is not a member of the group.",
            capitalize(&follower.definite_name(realm)?)
        ))
    }
}
