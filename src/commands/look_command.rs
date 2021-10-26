use crate::actions;
use crate::entity::EntityRef;
use crate::entity::Realm;
use crate::player_output::PlayerOutput;
use crate::relative_direction::RelativeDirection;
use crate::utils::{direction_by_abbreviation, is_direction, vector_for_direction};

use super::{inventory_command::inventory, CommandHelpers, CommandLineProcessor};

/// Makes the character look at *something*.
pub fn look(
    realm: &mut Realm,
    player_ref: EntityRef,
    helpers: CommandHelpers,
) -> Result<Vec<PlayerOutput>, String> {
    let (player, room) = realm.character_and_room_res(player_ref)?;

    let processor = helpers.command_line_processor;

    let alias = processor.take_word().unwrap();
    if alias.starts_with('l') {
        if !processor.has_words_left() {
            return actions::look_at_entity(realm, player_ref, player.current_room());
        }

        processor.skip_connecting_word("at");
    }

    if processor.peek_rest() == "inventory" || processor.peek_rest() == "in inventory" {
        return inventory(
            realm,
            player_ref,
            CommandHelpers {
                command_line_processor: &mut CommandLineProcessor::new(player_ref, "inventory"),
                ..helpers
            },
        );
    }

    let description = processor.take_entity_description().ok_or_else(|| {
        if alias == "examine" {
            "Examine what?".to_owned()
        } else {
            "Look at what?".to_owned()
        }
    })?;

    if processor.peek_rest() == "in inventory" {
        let entity_ref = processor
            .entity_by_description(realm, player.inventory(), description)
            .ok_or("You don't have that.")?;
        return actions::look_at_entity(realm, player_ref, entity_ref);
    }

    let pool = [
        &room.characters()[..],
        &room.items()[..],
        &room.portals()[..],
        &player.inventory()[..],
    ]
    .concat();

    let target = description.name.as_ref();
    let maybe_entity = processor
        .entity_by_description(realm, &pool, description.clone())
        .and_then(|entity_ref| realm.entity(entity_ref));
    let output = match maybe_entity {
        Some(entity) => {
            let mut output = Vec::<PlayerOutput>::new();
            if is_direction(target) {
                push_output_string!(output, player_ref, format!("You look {}.\n", target));
            } else if let Some(direction) = direction_by_abbreviation(target) {
                push_output_string!(output, player_ref, format!("You look {}.\n", direction));
            }
            output.append(&mut actions::look_at_entity(
                realm,
                player_ref,
                entity.entity_ref(),
            )?);
            output
        }
        None => {
            let (output, direction) = if let Some(direction) = direction_by_abbreviation(target) {
                let vector = vector_for_direction(direction).unwrap();
                (format!("You look {}.\n", direction), vector)
            } else if let Some(direction) = vector_for_direction(target) {
                (format!("You look {}.\n", target), direction)
            } else if let Some(relative_direction) = RelativeDirection::from_string(target) {
                (
                    match relative_direction {
                        RelativeDirection::Ahead => "You look ahead.\n",
                        RelativeDirection::Left => "You look to the left.\n",
                        RelativeDirection::Right => "You look to the right.\n",
                        RelativeDirection::Behind => "You look behind you.\n",
                        RelativeDirection::Up => "You look up.\n",
                        RelativeDirection::Down => "You look down.\n",
                    }
                    .to_owned(),
                    relative_direction.from(player.direction()),
                )
            } else {
                return Err(if alias == "examine" {
                    "Examine what?".to_owned()
                } else {
                    "Look where?".to_owned()
                });
            };

            let mut output = vec![PlayerOutput::new_from_string(player_ref.id(), output)];
            output.append(&mut actions::look_in_direction(
                realm, player_ref, &direction,
            )?);
            output
        }
    };

    if let Some(direction) = vector_for_direction(target) {
        actions::change_direction(realm, player_ref, direction);
    }

    Ok(output)
}
