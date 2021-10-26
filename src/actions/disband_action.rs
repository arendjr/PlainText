use crate::{
    entity::{EntityRef, Realm},
    player_output::PlayerOutput,
    utils::definite_character_name,
};

/// Disband the entire group (only the leader should do this).
pub fn disband(realm: &mut Realm, group_ref: EntityRef) -> Result<Vec<PlayerOutput>, String> {
    let group = realm.group(group_ref).ok_or("You're not in any group")?;
    let leader_ref = group.leader();
    let followers = group.followers().iter().copied().collect::<Vec<_>>();

    let mut output = vec![PlayerOutput::new_from_str(
        leader_ref.id(),
        "You disbanded the group.\n",
    )];

    let leader_name = definite_character_name(realm, leader_ref)?;

    for follower_ref in followers {
        if let Some(follower) = realm.character_mut(follower_ref) {
            follower.unset_group();
            output.push(PlayerOutput::new_from_string(
                follower_ref.id(),
                format!("{} has disbanded the group.\n", leader_name),
            ));
        }
    }

    if let Some(leader) = realm.character_mut(leader_ref) {
        leader.unset_group();
    }

    realm.unset(group_ref);

    Ok(output)
}
