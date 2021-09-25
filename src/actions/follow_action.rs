use crate::{
    actions,
    game_object::{GameObject, GameObjectRef},
    objects::Realm,
    player_output::PlayerOutput,
    sessions::SessionOutput,
    text_utils::capitalize,
};

/// Start following another character.
pub fn follow(
    realm: &mut Realm,
    follower_ref: GameObjectRef,
    leader_ref: GameObjectRef,
) -> Result<Vec<PlayerOutput>, String> {
    if follower_ref == leader_ref {
        return Err("You cannot follow yourself.".into());
    }

    let leader = realm.character_res(leader_ref)?;
    let follower = realm.character_res(follower_ref)?;

    // If players can follow NPCs this easily, it would be no challenge:
    if follower.is_player() && !leader.is_player() {
        return Err(format!(
            "You cannot follow {}.",
            leader.definite_name(realm)?
        ));
    }

    let mut output = vec![];
    let maybe_existing_group = leader.group();

    if let Some(group) = follower
        .group()
        .and_then(|group_ref| realm.group(group_ref))
    {
        if group.leader() == leader_ref {
            return Err(format!("You are already following {}.", leader.name()));
        }
        if group.followers().contains(&leader_ref) {
            return Err(format!(
                "You are already in the same group as {}.",
                leader.name()
            ));
        }

        output.append(&mut unfollow(realm, follower_ref)?);
    }

    let group_ref = match maybe_existing_group {
        Some(group_ref) => group_ref,
        None => realm.create_group(leader_ref),
    };

    if let Some(group) = realm.group_mut(group_ref) {
        group.add_follower(follower_ref);
    }

    if let Some(follower) = realm.character_mut(follower_ref) {
        follower.set_group(group_ref);
    }

    output.append(&mut create_follow_output(
        realm,
        group_ref,
        leader_ref,
        follower_ref,
    )?);
    Ok(output)
}

fn create_follow_output(
    realm: &Realm,
    group_ref: GameObjectRef,
    leader_ref: GameObjectRef,
    follower_ref: GameObjectRef,
) -> Result<Vec<PlayerOutput>, String> {
    let group = realm.group(group_ref).ok_or("Unknown group")?;
    let leader = realm.character_res(leader_ref)?;
    let follower = realm.character_res(follower_ref)?;

    let mut output = vec![];
    let follower_name = follower.definite_name(realm)?;

    if group.followers().len() == 1 {
        output.push(PlayerOutput::new_from_string(
            follower.id(),
            format!("You start following {}.\n", leader.name()),
        ));
        output.push(PlayerOutput::new_from_string(
            leader.id(),
            format!("{} started following you.\n", capitalize(&follower_name)),
        ));
    } else {
        let group_leader = realm.character_res(group.leader())?;
        output.push(PlayerOutput::new_from_string(
            follower.id(),
            format!(
                "You joined the group of {}, led by {}.\n",
                leader.name(),
                group_leader.name()
            ),
        ));
        output.append(&mut group.send_all(SessionOutput::String(format!(
            "{} joined your group.\n",
            capitalize(&follower_name)
        ))));
    }

    Ok(output)
}

/// Stop following anyone else.
pub fn unfollow(
    realm: &mut Realm,
    follower_ref: GameObjectRef,
) -> Result<Vec<PlayerOutput>, String> {
    let follower = realm
        .character(follower_ref)
        .ok_or("Your account has been deactivated.")?;
    let group = follower
        .group()
        .and_then(|group| realm.group(group))
        .ok_or("You're not in any group.")?;

    let group_ref = group.object_ref();
    let leader_ref = group.leader();

    if leader_ref == follower_ref {
        return actions::disband(realm, group_ref);
    }

    if group.followers().len() > 1 {
        if let Some(group) = realm.group_mut(group_ref) {
            group.remove_follower(follower_ref);
        }
    } else if let Some(leader) = realm.character_mut(leader_ref) {
        leader.unset_group();
        realm.unset(group_ref);
    }

    if let Some(follower) = realm.character_mut(follower_ref) {
        follower.unset_group();
    }

    create_unfollow_output(realm, group_ref, leader_ref, follower_ref)
}

fn create_unfollow_output(
    realm: &Realm,
    group_ref: GameObjectRef,
    leader_ref: GameObjectRef,
    follower_ref: GameObjectRef,
) -> Result<Vec<PlayerOutput>, String> {
    let group = realm.group(group_ref).ok_or("Unknown group")?;
    let leader = realm.character_res(leader_ref)?;
    let follower = realm.character_res(follower_ref)?;

    let mut output = vec![];

    if group.followers().is_empty() {
        output.push(PlayerOutput::new_from_string(
            follower.id(),
            format!("You stopped following {}.\n", leader.definite_name(realm)?),
        ));
        output.push(PlayerOutput::new_from_string(
            leader.id(),
            format!(
                "{} has stopped following you.\n",
                capitalize(&follower.definite_name(realm)?)
            ),
        ));
    } else {
        output.push(PlayerOutput::new_from_str(
            follower.id(),
            "You left the group.\n",
        ));
        output.append(&mut group.send_all(SessionOutput::String(format!(
            "{} left the group.",
            capitalize(&follower.definite_name(realm)?)
        ))));
    }

    Ok(output)
}
