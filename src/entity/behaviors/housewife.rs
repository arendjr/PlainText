use super::{Actor, Behavior};
use crate::{
    actionable_events::{ActionDispatcher, ActionableEvent},
    actions::{self, ActionOutput},
    entity::{Entity, EntityRef, EntityType, Item, ItemFlags, Realm},
    player_output::PlayerOutput,
    utils::{capitalize, definite_character_name, named_portal},
};
use std::time::Duration;

const EMPTY_DESCRIPTION: &str = "She's carrying an empty basket.";
const FILLING_DESCRIPTION: &str = "She's busy getting water from the well.";
const FULL_DESCRIPTION: &str = "She's carrying a bucket full of water.";

pub struct Housewife {
    entity_ref: EntityRef,
}

impl Housewife {
    pub fn new(entity_ref: EntityRef) -> Self {
        Self { entity_ref }
    }
}

impl Housewife {
    fn schedule_activate_after(
        &self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        duration: Duration,
    ) {
        if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
            if let Some(abort_handle) = actor_state.abort_handle.as_ref() {
                abort_handle.abort();
            }

            actor_state.abort_handle = Some(dispatcher.dispatch_cancelable_after(
                ActionableEvent::ActivateActor(self.entity_ref),
                duration,
            ));
        }
    }
}

impl Actor for Housewife {
    fn on_active(&self, realm: &mut Realm, dispatcher: &ActionDispatcher) -> ActionOutput {
        self.schedule_activate_after(realm, dispatcher, Duration::from_millis(27768));

        let housewife = realm.npc(self.entity_ref).ok_or("No such character")?;
        let room = realm
            .room(housewife.character.current_room())
            .ok_or("No such room")?;
        let room_ref = room.entity_ref();
        let bucket_ref = housewife.character.inventory().first().copied();
        let characters = room.characters().clone();
        let description = housewife.description().to_owned();
        if !characters.contains(&self.entity_ref) {
            return self.on_spawn(realm, dispatcher);
        }

        let present_enemy_ref = characters
            .iter()
            .copied()
            .find(|character_ref| housewife.actor_state.enemies.contains(character_ref));
        if let Some(enemy_ref) = present_enemy_ref {
            let is_guard_present = characters
                .iter()
                .filter_map(|character_ref| realm.npc(*character_ref))
                .any(|npc| matches!(npc.behavior(), Some(Behavior::Guard)));
            return if is_guard_present {
                if housewife.actor_state.get_misc_bool("pointed") {
                    Ok(Vec::new()) // Already pointed.
                } else {
                    if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
                        actor_state.set_misc_bool("pointed", true);
                    }

                    let housewife_name = definite_character_name(realm, self.entity_ref)?;
                    let enemy_name = definite_character_name(realm, enemy_ref)?;
                    Ok(characters
                        .iter()
                        .map(|character_ref| {
                            PlayerOutput::new_from_string(
                                character_ref.id(),
                                format!("{} points to {}.", housewife_name, enemy_name),
                            )
                        })
                        .collect())
                }
            } else {
                actions::shout(realm, self.entity_ref, "Help! Guards!")
            };
        }

        if housewife.description() == EMPTY_DESCRIPTION {
            let is_cat_present = room
                .characters()
                .iter()
                .filter_map(|character_ref| realm.npc(*character_ref))
                .any(|npc| npc.name() == "cat");
            if is_cat_present {
                if housewife.actor_state.get_misc_bool("petted") {
                    // Already petted.
                } else {
                    if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
                        actor_state.set_misc_bool("petted", true);
                    }

                    let housewife_name = definite_character_name(realm, self.entity_ref)?;
                    return Ok(characters
                        .iter()
                        .map(|character_ref| {
                            PlayerOutput::new_from_string(
                                character_ref.id(),
                                format!("{} pats the cat.", housewife_name),
                            )
                        })
                        .collect());
                }
            }
        }

        if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
            actor_state.set_misc_bool("petted", false);
        }

        match room_ref.id() {
            132 => {
                if description == EMPTY_DESCRIPTION {
                    let portal_ref =
                        named_portal(realm, room_ref, "west").ok_or("Don't know where to go")?;
                    actions::enter_portal(realm, dispatcher, self.entity_ref, portal_ref, room_ref)
                } else {
                    actions::leave_room(realm, self.entity_ref)
                }
            }
            113 => {
                let portal_name = if description == EMPTY_DESCRIPTION {
                    "west"
                } else {
                    "east"
                };
                let portal_ref =
                    named_portal(realm, room_ref, portal_name).ok_or("Don't know where to go")?;
                actions::enter_portal(realm, dispatcher, self.entity_ref, portal_ref, room_ref)
            }
            110 => {
                if description == EMPTY_DESCRIPTION {
                    if let Some(housewife) = realm.npc_mut(self.entity_ref) {
                        housewife.set_description(FILLING_DESCRIPTION.to_owned());
                    }
                    Ok(Vec::new())
                } else if description == FILLING_DESCRIPTION {
                    if let Some(bucket) = bucket_ref.and_then(|item_ref| realm.item_mut(item_ref)) {
                        bucket.set_name("bucket full of water".to_owned());
                        bucket.set_plural_form("buckets full of water".to_owned());
                        bucket.set_description("It's a bucket. It's filled with water.".to_owned());
                        bucket.set_weight(10.0);
                    }
                    if let Some(housewife) = realm.npc_mut(self.entity_ref) {
                        housewife.set_description(FULL_DESCRIPTION.to_owned());
                    }
                    Ok(Vec::new())
                } else {
                    let portal_ref =
                        named_portal(realm, room_ref, "east").ok_or("Don't know where to go")?;
                    actions::enter_portal(realm, dispatcher, self.entity_ref, portal_ref, room_ref)
                }
            }
            _ => Ok(Vec::new()),
        }
    }

    fn on_attack(
        &self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        attacker: EntityRef,
    ) -> ActionOutput {
        if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
            actor_state.enemies.insert(attacker);
        }

        self.schedule_activate_after(realm, dispatcher, Duration::from_millis(150));

        Ok(Vec::new())
    }

    fn on_character_died(
        &self,
        realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        casualty: EntityRef,
        _attacker: Option<EntityRef>,
    ) -> ActionOutput {
        if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
            actor_state.enemies.remove(&casualty);
        }

        Ok(Vec::new())
    }

    fn on_die(
        &self,
        realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        _attacker: Option<EntityRef>,
    ) -> ActionOutput {
        if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
            actor_state.enemies.clear();
        }

        Ok(Vec::new())
    }

    fn on_spawn(&self, realm: &mut Realm, dispatcher: &ActionDispatcher) -> ActionOutput {
        let housewife = realm.npc_mut(self.entity_ref).ok_or("No such character")?;
        housewife.actor_state.enemies.clear();
        housewife
            .character
            .set_current_room(EntityRef::new(EntityType::Room, 132));
        housewife.set_description(EMPTY_DESCRIPTION.to_owned());

        if housewife.character.inventory().is_empty() {
            let mut bucket = Item::new(realm.next_id(), "empty bucket".to_owned());
            bucket.set_description("It's a bucket. It's empty.".to_owned());
            bucket.set_flags(ItemFlags::Portable);
            bucket.set_weight(1.0);
            let bucket_ref = bucket.entity_ref();
            realm.set(bucket_ref, Box::new(bucket));

            if let Some(housewife) = realm.npc_mut(self.entity_ref) {
                housewife.character.set_inventory(vec![bucket_ref]);
            }
        }

        self.schedule_activate_after(realm, dispatcher, Duration::from_millis(27768));

        actions::enter_current_room(realm, self.entity_ref)
    }

    fn on_talk(
        &self,
        realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        speaker: EntityRef,
        _message: &str,
    ) -> ActionOutput {
        let name = definite_character_name(realm, self.entity_ref)?;

        Ok(vec![PlayerOutput::new_from_string(
            speaker.id(),
            format!("{} pretends not to hear you.\n", capitalize(&name)),
        )])
    }
}

impl<'a> std::fmt::Debug for Housewife {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_str("Housewife")
    }
}
