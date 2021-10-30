use super::ActionOutput;
use crate::{
    actionable_events::ActionDispatcher,
    actions,
    character_stats::CharacterStat,
    colors::Color,
    entity::{CharacterAction, Entity, EntityRef, EntityType, Realm},
    player_output::PlayerOutput,
    utils::{
        capitalize, colorize, definite_character_name, object_pronoun, possessive_adjective,
        subject_pronoun, substring, total_stats,
    },
};
use std::time::{Duration, SystemTime};

/// Attacks another character.
pub fn kill(
    realm: &mut Realm,
    dispatcher: &ActionDispatcher,
    attacker_ref: EntityRef,
    defendant_ref: EntityRef,
) -> ActionOutput {
    let (attacker, room) = realm.character_and_room_res(attacker_ref)?;
    let defendant = realm.character_res(defendant_ref)?;

    if defendant.current_room() != attacker.current_room() {
        return Err(format!(
            "{} is not here.",
            capitalize(&definite_character_name(realm, defendant_ref)?)
        ));
    }

    if let Some(stun_message) = attacker.stun_message() {
        return Err(colorize(&stun_message, Color::Olive));
    }

    let characters = room.characters().clone();

    let mut output = Vec::new();

    if let Some(actor) = realm.actor(defendant_ref) {
        output.append(&mut actor.borrow().on_attack(realm, dispatcher, attacker_ref)?);
    }

    output.append(&mut do_combat(
        realm,
        dispatcher,
        attacker_ref,
        defendant_ref,
    )?);

    for character_ref in characters {
        if character_ref != attacker_ref && character_ref != defendant_ref {
            if let Some(actor) = realm.actor(character_ref) {
                output.append(&mut actor.borrow().on_character_attacked(
                    realm,
                    dispatcher,
                    attacker_ref,
                    defendant_ref,
                )?);
            }
        }
    }

    if let Some(defender) = realm.character(defendant_ref) {
        if defender.hp() <= 0 {
            output.append(&mut actions::die(
                realm,
                dispatcher,
                defendant_ref,
                Some(attacker_ref),
            )?);
        }
    }

    Ok(output)
}

fn do_combat(
    realm: &mut Realm,
    action_dispatcher: &ActionDispatcher,
    attacker_ref: EntityRef,
    defendant_ref: EntityRef,
) -> ActionOutput {
    use CharacterStat::*;

    let attacker_stats = total_stats(realm, attacker_ref);
    let defendant_stats = total_stats(realm, defendant_ref);

    let hit_chance = 256.0
        * ((80 + attacker_stats.get(Dexterity)) as f32 / 160.0)
        * ((100 - defendant_stats.get(Dexterity)) as f32 / 100.0);

    let damage = if rand::random::<u8>() as f32 > hit_chance {
        let max_damage = 20.0
            * (attacker_stats.get(Strength) as f32 / 40.0)
            * ((80 - defendant_stats.get(Endurance)) as f32 / 80.0);
        1 + (rand::random::<u8>() as f32 * max_damage / 256.0) as i16
    } else {
        0
    };

    if damage > 0 {
        if let Some(defendant) = realm.character_mut(defendant_ref) {
            defendant.set_hp(defendant.hp() - damage);
        }
    }

    let action_duration = Duration::from_millis(4000 - 25 * attacker_stats.get(Dexterity) as u64);
    actions::set_character_action(
        realm,
        action_dispatcher,
        attacker_ref,
        CharacterAction::Fighting {
            target: defendant_ref,
            end_time: SystemTime::now() + action_duration,
        },
        action_duration,
    );

    let message_template = get_combat_message_template(realm, attacker_ref, defendant_ref, damage);
    get_personalized_output(realm, attacker_ref, defendant_ref, damage, message_template)
}

fn get_combat_message_template(
    realm: &Realm,
    attacker_ref: EntityRef,
    defendant_ref: EntityRef,
    damage: i16,
) -> (String, String) {
    let (attacker, defendant) = match (
        realm.character(attacker_ref),
        realm.character(defendant_ref),
    ) {
        (Some(attacker), Some(defendant)) => (attacker, defendant),
        _ => panic!("Missing combatant"),
    };

    let defendant_is_lightweight_animal = if let Some(race) = realm.race(defendant.race()) {
        race.name() == "animal" && defendant.weight() < 50.0
    } else {
        false
    };

    let weapon_category = ""; // TODO: (attacker.weapon ? attacker.weapon.category : "");
    let secondary_weapon_category = ""; // TODO: (attacker.secondaryWeapon ? attacker.secondaryWeapon.category : "");

    let attacker_weapon = |category: &str| -> Option<&dyn Entity> {
        /* TODO: if weapon_category == category {
            Some(attacker.weapon)
        } else if secondary_weapon_category == category {
            Some(attacker.secondaryWeapon)
        } else*/
        {
            None
        }
    };

    let attacker_is_wielding = |category: &str| attacker_weapon(category).is_some();

    let mut beginnings = vec![];
    let mut endings = vec![];

    if defendant_is_lightweight_animal {
        beginnings.push("%a kicks toward %d".to_owned());
        if damage > 0 {
            if let Some(weapon) = attacker_weapon("stick") {
                beginnings.push(format!("%a smashes %pa {} into %d", weapon.name()));
                beginnings.push("%a bludgeons %d".to_owned());
            }

            endings.push("and %d jumps up as you hit %pd chest".to_owned());
        } else {
            beginnings.push("%a tries to hit %d".to_owned());

            endings.push("but %d is too fast for %oa.".to_owned());
        }
    } else {
        if weapon_category.is_empty() && secondary_weapon_category.is_empty() {
            beginnings.push("%a brings up a mighty punch".to_owned());
            beginnings.push("%a kicks wildly at %d".to_owned());
        } else {
            beginnings.push("%a thrusts at %d".to_owned());

            if attacker_is_wielding("sword") {
                beginnings.push("%a slashes at %d".to_owned());
            } else if attacker_is_wielding("spear") {
                beginnings.push("%a lashes out at %d".to_owned());
            } else if attacker_is_wielding("dagger") {
                beginnings.push("%a stabs at %d".to_owned());
            }
        }

        if damage > 0 {
            if weapon_category.is_empty() && secondary_weapon_category.is_empty() {
                beginnings.push("%a deals %d a sweeping punch".to_owned());
            } else if let Some(weapon) = attacker_weapon("stick") {
                beginnings.push(format!("%a smashes %pa {} into %d", weapon.name()));
                beginnings.push("%a bludgeons %d".to_owned());
            } else if let Some(weapon) = attacker_weapon("warhammer") {
                beginnings.push(format!("%a smashes %pa {} into %d", weapon.name()));
                beginnings.push("%a bludgeons %d".to_owned());
                beginnings.push(format!("%a crushes %d with %pa {}", weapon.name()));
            }

            if attacker.height() > defendant.height() - 100.0 {
                endings.push("and hits %od in the face".to_owned());
                endings.push("and violently smashes %pd nose".to_owned());
                endings.push("hitting %od on the jaw".to_owned());
            }

            endings.push("and hits %od in the stomach, causing %od to double over".to_owned());
            endings.push("and hits %od in the flank".to_owned());
        } else {
            if weapon_category.is_empty() && secondary_weapon_category.is_empty() {
                beginnings.push("%a tries to hit %d".to_owned());
            } else {
                beginnings.push("%a tries to hit %d with %pa %w".to_owned());
            }

            endings.push("but %d blocks the blow".to_owned());
            endings.push("but hits nothing but air".to_owned());
            endings.push("but fails to hurt %od".to_owned());
        }
    }

    let beginning_index = rand::random::<usize>() % beginnings.len();
    let beginning = beginnings.into_iter().nth(beginning_index).unwrap();
    let ending_index = rand::random::<usize>() % endings.len();
    let ending = endings.into_iter().nth(ending_index).unwrap();
    (beginning, ending)
}

fn get_personalized_output(
    realm: &Realm,
    attacker_ref: EntityRef,
    defendant_ref: EntityRef,
    damage: i16,
    (beginning, ending): (String, String),
) -> Result<Vec<PlayerOutput>, String> {
    let message = if ending.starts_with("and") {
        format!("{} {}", beginning, ending)
    } else {
        format!("{}, {}", beginning, ending)
    };

    let second_person_indicative_of = |verb: &str| {
        if verb.ends_with("ies") {
            format!("{}y", substring(verb, 0, -3))
        } else if verb.ends_with("es") {
            substring(verb, 0, -2).to_owned()
        } else {
            substring(verb, 0, -1).to_owned()
        }
    };

    let verb = beginning.split(' ').nth(1).unwrap();
    let second_person_indicative = second_person_indicative_of(verb);

    let mut ending_words = ending.split(' ');
    let (ending_verb, ending_subject) = match (ending_words.nth(1), ending_words.next()) {
        (Some(second_word), _) if second_word.ends_with('s') => (second_word, Subject::Attacker),
        (Some(second_word), Some(third_word)) if third_word.ends_with('s') => (
            third_word,
            if second_word == "%d" {
                Subject::Defendant
            } else {
                Subject::Attacker
            },
        ),
        _ => ("", Subject::Unknown),
    };
    let ending_second_person_indicative = if ending_verb.is_empty() {
        "".to_owned()
    } else {
        second_person_indicative_of(ending_verb)
    };

    let attacker_definite_name = definite_character_name(realm, attacker_ref)?;
    let defendant_definite_name = definite_character_name(realm, defendant_ref)?;

    let mut output = Vec::new();

    if attacker_ref.entity_type() == EntityType::Player {
        let mut attacker_message = message.replace(verb, &second_person_indicative);
        if ending_subject == Subject::Attacker {
            attacker_message =
                attacker_message.replace(ending_verb, &ending_second_person_indicative);
        }
        attacker_message = attacker_message
            .replace("%a", "you")
            .replace("%oa", "you")
            .replace("%pa", "your");
        if attacker_message.contains("%d") {
            attacker_message = attacker_message
                .replacen("%d", &defendant_definite_name, 1)
                .replace("%d", subject_pronoun(realm, defendant_ref))
                .replace("%od", object_pronoun(realm, defendant_ref))
                .replace("%pd", possessive_adjective(realm, defendant_ref));
        } else {
            attacker_message = attacker_message
                .replacen("%od", &defendant_definite_name, 1)
                .replace("%od", object_pronoun(realm, defendant_ref))
                .replace("%pd", &format!("{}'s", defendant_definite_name));
        }
        if damage > 0 {
            attacker_message += &format!(", dealing {} damage.\n", damage);
        } else {
            attacker_message += ".\n";
        }

        output.push(PlayerOutput::new_from_string(
            attacker_ref.id(),
            colorize(&capitalize(&attacker_message), Color::Teal),
        ));
    }

    if defendant_ref.entity_type() == EntityType::Player {
        let mut defendant_message = message.clone();
        if ending_subject == Subject::Defendant {
            defendant_message =
                defendant_message.replace(ending_verb, &ending_second_person_indicative);
        }
        defendant_message = defendant_message
            .replace("%a", &attacker_definite_name)
            .replace("%oa", object_pronoun(realm, attacker_ref))
            .replace("%pa", possessive_adjective(realm, attacker_ref))
            .replace("%d", "you")
            .replace("%od", "you")
            .replace("%pd", "your");
        if damage > 0 {
            defendant_message += &format!(", dealing {} damage.\n", damage);
        } else {
            defendant_message += ".\n";
        }

        output.push(PlayerOutput::new_from_string(
            defendant_ref.id(),
            colorize(
                &capitalize(&defendant_message),
                if damage > 0 { Color::Red } else { Color::Green },
            ),
        ));
    }

    let (_, room) = realm.character_and_room_res(attacker_ref)?;
    let observers: Vec<EntityRef> = room
        .characters()
        .iter()
        .copied()
        .filter(|character_ref| *character_ref != attacker_ref && *character_ref != defendant_ref)
        .collect();
    if !observers.is_empty() {
        let mut observers_message = format!("{}.\n", message)
            .replace("%a", &attacker_definite_name)
            .replace("%oa", object_pronoun(realm, attacker_ref))
            .replace("%pa", possessive_adjective(realm, attacker_ref));
        if observers_message.contains("%d") {
            observers_message = observers_message
                .replacen("%d", &defendant_definite_name, 1)
                .replace("%d", subject_pronoun(realm, defendant_ref))
                .replace("%od", object_pronoun(realm, defendant_ref))
                .replace("%pd", possessive_adjective(realm, defendant_ref));
        } else {
            observers_message = observers_message
                .replace("%od", &defendant_definite_name)
                .replace("%pd", &format!("{}'s", defendant_definite_name));
        }

        for observer_ref in observers {
            output.push(PlayerOutput::new_from_string(
                observer_ref.id(),
                colorize(&capitalize(&observers_message), Color::Teal),
            ));
        }
    }

    Ok(output)
}

#[derive(PartialEq)]
enum Subject {
    Unknown,
    Attacker,
    Defendant,
}
