use crate::{game_object::GameObjectRef, objects::Realm, player_output::PlayerOutput};

/// Disband the entire group (only the leader should do this).
pub fn disband(realm: &mut Realm, group_ref: GameObjectRef) -> Result<Vec<PlayerOutput>, String> {
    let group = realm.group(group_ref).ok_or("You're not in any group")?;
    let leader = group.leader();
    let followers = group.followers().iter().copied().collect::<Vec<_>>();

    let mut output = vec![];

    if let Some(leader) = realm.character(leader) {
        output.push(PlayerOutput::new_from_str(
            leader.id(),
            "You disbanded the group.\n",
        ));

        let leader_name = leader.definite_name(realm)?;
        let leader_ref = leader.object_ref();

        for follower in followers {
            if let Some(follower) = realm.character_mut(follower) {
                follower.unset_group();
                output.push(PlayerOutput::new_from_string(
                    follower.id(),
                    format!("{} has disbanded the group.\n", leader_name),
                ));
            }
        }

        if let Some(leader) = realm.character_mut(leader_ref) {
            leader.unset_group();
        }
    }

    realm.unset(group_ref);

    Ok(output)
}
