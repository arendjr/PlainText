use super::ActionOutput;
use crate::{
    actionable_events::{ActionDispatcher, ActionableEvent},
    actions,
    colors::Color,
    entity::{Entity, EntityRef, EntityType, Realm},
    player_output::PlayerOutput,
    utils::{capitalize, colorize, definite_character_name, describe_items, join_sentence},
};
use std::time::Duration;

/// Makes the character die.
pub fn die(
    realm: &mut Realm,
    dispatcher: &ActionDispatcher,
    character_ref: EntityRef,
    maybe_attacker_ref: Option<EntityRef>,
) -> ActionOutput {
    let mut output = vec![PlayerOutput::new_from_string(
        character_ref.id(),
        colorize("You died.\n", Color::Red),
    )];

    if let Some(actor) = realm.actor(character_ref) {
        output.append(
            &mut actor
                .borrow()
                .on_die(realm, dispatcher, maybe_attacker_ref)?,
        );
    }

    let (character, room) = realm.character_and_room_res(character_ref)?;
    let character_name = capitalize(&definite_character_name(realm, character_ref)?);
    let current_room_ref = room.entity_ref();
    let inventory = character.inventory().clone();
    let group_ref = character.group();
    let race_ref = character.race();

    let others = room
        .characters()
        .iter()
        .copied()
        .filter(|other_ref| *other_ref != character_ref)
        .collect::<Vec<_>>();
    for other in others.iter() {
        output.push(PlayerOutput::new_from_string(
            other.id(),
            colorize(&format!("{} died.\n", character_name), Color::Teal),
        ));
    }

    if !inventory.is_empty() {
        if let Some(room) = realm.room_mut(current_room_ref) {
            room.add_items(&inventory);
        }
        if let Some(character) = realm.character_mut(character_ref) {
            character.set_inventory(Vec::new());
        }

        let dropped_items_description = join_sentence(&describe_items(realm, &inventory));
        for other in others.iter() {
            output.push(PlayerOutput::new_from_string(
                other.id(),
                colorize(
                    &format!(
                        "{} was carrying {}.\n",
                        character_name, dropped_items_description
                    ),
                    Color::Teal,
                ),
            ));
        }
    }

    for other in others {
        if let Some(actor) = realm.actor(other) {
            actor.borrow().on_character_died(
                realm,
                dispatcher,
                character_ref,
                maybe_attacker_ref,
            )?;
        }
    }

    if group_ref.is_some() {
        actions::unfollow(realm, character_ref)?;
    }

    if let Some(room) = realm.room_mut(current_room_ref) {
        room.remove_characters(&[character_ref]);
    }

    if character_ref.entity_type() == EntityType::Player {
        // TODO: LogUtil.countPlayerDeath(room.toString());

        let starting_room = match realm.race(race_ref) {
            Some(race) => race.starting_room(),
            None => current_room_ref,
        };
        if let Some(character) = realm.character_mut(character_ref) {
            character.set_current_room(starting_room);
            character.set_hp(1);
        }

        actions::enter_current_room(realm, character_ref)?;
        actions::stun(realm, dispatcher, character_ref, Duration::from_secs(5));
        output.append(&mut actions::look_at_entity(
            realm,
            character_ref,
            starting_room,
        )?);
    } else if let Some(respawnable) = realm.respawnable(character_ref) {
        let respawn_time = respawnable.random_respawn_time();
        dispatcher.dispatch_after(ActionableEvent::SpawnNpc(character_ref), respawn_time);
    } else {
        realm.unset(character_ref);
    }

    Ok(output)
}
