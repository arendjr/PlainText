use super::Actor;
use crate::{
    actionable_events::{ActionDispatcher, ActionableEvent},
    actions,
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
    fn on_active(&self, realm: &mut Realm, dispatcher: &ActionDispatcher) -> Vec<PlayerOutput> {
        if let Some(guard) = realm.npc(self.entity_ref) {
            for enemy_ref in guard.actor_state.enemies.iter() {
                if let Some(enemy) = realm.character(*enemy_ref) {
                    if enemy.current_room() == guard.character.current_room() {
                        return actions::kill(realm, dispatcher, self.entity_ref, *enemy_ref)
                            .unwrap_or_default();
                    }
                }
            }
        }

        Vec::new()
    }

    fn on_attack(
        &self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        attacker: EntityRef,
    ) -> Vec<PlayerOutput> {
        if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
            actor_state.enemies.insert(attacker);
        }

        dispatcher.dispatch_after(
            ActionableEvent::ActivateActor(self.entity_ref),
            Duration::from_millis(200),
        );

        Vec::new()
    }

    fn on_character_attacked(
        &self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        attacker_ref: EntityRef,
        _defendent: EntityRef,
    ) -> Vec<PlayerOutput> {
        if let Some(attacker) = realm.entity(attacker_ref) {
            if attacker.name() != "city guard" {
                if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
                    actor_state.enemies.insert(attacker_ref);
                }

                dispatcher.dispatch_after(
                    ActionableEvent::ActivateActor(self.entity_ref),
                    Duration::from_millis(600),
                );
            }
        }

        Vec::new()
    }

    fn on_character_died(
        &self,
        realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        casualty: EntityRef,
        _: Option<EntityRef>,
    ) -> Vec<PlayerOutput> {
        if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
            actor_state.enemies.remove(&casualty);
        }

        Vec::new()
    }

    fn on_character_entered(
        &mut self,
        _realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        _character: EntityRef,
    ) -> Vec<PlayerOutput> {
        dispatcher.dispatch_after(
            ActionableEvent::ActivateActor(self.entity_ref),
            Duration::from_millis(300),
        );

        Vec::new()
    }

    fn on_die(
        &self,
        realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        _attacker: Option<EntityRef>,
    ) -> Vec<PlayerOutput> {
        if let Some(actor_state) = realm.actor_state_mut(self.entity_ref) {
            actor_state.enemies.clear();
        }

        Vec::new()
    }

    fn on_spawn(&self, realm: &mut Realm, _dispatcher: &ActionDispatcher) -> Vec<PlayerOutput> {
        if let Some(guard) = realm.character_mut(self.entity_ref) {
            guard.set_indefinite_action(CharacterAction::Guarding {
                target: EntityRef(EntityType::Portal, 1507),
            });
        }

        Vec::new()
    }

    fn on_talk(
        &self,
        _realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        talker: EntityRef,
        _message: &str,
    ) -> Vec<PlayerOutput> {
        vec![PlayerOutput::new_from_str(
            talker.id(),
            "The guard looks at you and shrugs.\n",
        )]
    }
}

impl<'a> std::fmt::Debug for Guard {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.write_str("Guard")
    }
}
