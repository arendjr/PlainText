use crate::{
    actionable_events::ActionDispatcher,
    entity::{EntityRef, Realm},
    player_output::PlayerOutput,
};

pub trait Actor: std::fmt::Debug {
    /// Activates an actor.
    ///
    /// Actors are responsible for scheduling their own activation based on any of the other
    /// triggers in this trait. The only time `on_active()` is called automatically is when
    /// the actor becomes idle again after performing some other action.
    fn on_active(&self, realm: &mut Realm, dispatcher: &ActionDispatcher) -> Vec<PlayerOutput>;

    /// Invoked when the actor itself is being attacked.
    fn on_attack(
        &self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        attacker: EntityRef,
    ) -> Vec<PlayerOutput>;

    /// Invoked when a character in the same room is being attacked.
    fn on_character_attacked(
        &self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        attacker: EntityRef,
        defendent: EntityRef,
    ) -> Vec<PlayerOutput>;

    /// Invoked when a character in the same room dies.
    ///
    /// If the character dies as a direct result of another character's attack, the attacker
    /// is given as well.
    fn on_character_died(
        &self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        casualty: EntityRef,
        attacker: Option<EntityRef>,
    ) -> Vec<PlayerOutput>;

    /// Invoked when a character entered the same room.
    fn on_character_entered(
        &mut self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        character: EntityRef,
    ) -> Vec<PlayerOutput>;

    /// Invoked when the character itself dies.
    ///
    /// If the character dies as a direct result of another character's attack, the attacker
    /// is given as well.
    fn on_die(
        &self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        attacker: Option<EntityRef>,
    ) -> Vec<PlayerOutput>;

    /// Invoked when the actor is spawned.
    fn on_spawn(&self, realm: &mut Realm, dispatcher: &ActionDispatcher) -> Vec<PlayerOutput>;

    /// Invoked when a character talks directly to the actor.
    fn on_talk(
        &self,
        realm: &mut Realm,
        dispatcher: &ActionDispatcher,
        talker: EntityRef,
        message: &str,
    ) -> Vec<PlayerOutput>;
}
