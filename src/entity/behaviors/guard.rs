use super::{Actor, Behavior};
use crate::{
    actionable_events::{ActionDispatcher, ActionableEvent},
    actions::{self, ActionOutput},
    entity::{CharacterAction, EntityRef, EntityType, Realm},
    player_output::PlayerOutput,
};
use std::time::Duration;

pub struct Guard {
    entity_ref: EntityRef,
}

impl Guard {
    pub fn new(entity_ref: EntityRef) -> Self {
        Self { entity_ref }
    }
}

impl Actor for Guard {
    fn activate_on_idle(&self) -> bool {
        true
    }

    fn on_active(&self, realm: &mut Realm, dispatcher: &ActionDispatcher) -> ActionOutput {
        let enemy_in_same_room = realm.npc(self.entity_ref).and_then(|guard| {
            guard
                .actor_state
                .enemies
                .iter()
                .filter_map(|enemy_ref| realm.character(*enemy_ref).map(|enemy| (enemy_ref, enemy)))
                .find_map(|(enemy_ref, enemy)| {
                    if enemy.current_room() == guard.character.current_room() {
                        Some(*enemy_ref)
                    } else {
                        None
                    }
                })
        });
        if let Some(enemy_ref) = enemy_in_same_room {
            return actions::kill(realm, dispatcher, self.entity_ref, enemy_ref);
        }

        Ok(Vec::new())
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

        dispatcher.dispatch_after(
            ActionableEvent::ActivateActor(self.entity_ref),
            Duration::from_millis(200),
        );

        Ok(Vec::new())
    }

    fn on_character_attacked(
        &self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        attacker_ref: EntityRef,
        _defendent: EntityRef,
    ) -> ActionOutput {
        if !realm
            .npc(attacker_ref)
            .map(|npc| matches!(npc.behavior(), Some(Behavior::Guard)))
            .unwrap_or_default()
        {
            if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
                actor_state.enemies.insert(attacker_ref);
            }

            dispatcher.dispatch_after(
                ActionableEvent::ActivateActor(self.entity_ref),
                Duration::from_millis(600),
            );
        }

        Ok(Vec::new())
    }

    fn on_character_died(
        &self,
        realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        casualty: EntityRef,
        _: Option<EntityRef>,
    ) -> ActionOutput {
        if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
            actor_state.enemies.remove(&casualty);
        }

        Ok(Vec::new())
    }

    fn on_character_entered(
        &mut self,
        _realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        _character: EntityRef,
    ) -> ActionOutput {
        dispatcher.dispatch_after(
            ActionableEvent::ActivateActor(self.entity_ref),
            Duration::from_millis(300),
        );

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

    fn on_spawn(&self, realm: &mut Realm, _dispatcher: &ActionDispatcher) -> ActionOutput {
        let guard = realm
            .character_mut(self.entity_ref)
            .ok_or("No such character")?;
        guard.set_indefinite_action(CharacterAction::Guarding {
            target: EntityRef(EntityType::Portal, 1507),
        });

        Ok(Vec::new())
    }

    fn on_talk(
        &self,
        _realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        speaker: EntityRef,
        _message: &str,
    ) -> ActionOutput {
        Ok(vec![PlayerOutput::new_from_str(
            speaker.id(),
            "The guard looks at you and shrugs.\n",
        )])
    }
}

impl<'a> std::fmt::Debug for Guard {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_str("Guard")
    }
}
