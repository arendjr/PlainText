use crate::{
    actionable_events::ActionDispatcher,
    actions::ActionOutput,
    entity::{EntityRef, Realm},
};

pub trait Actor: std::fmt::Debug {
    /// Returns whether the actor's `on_active()` callback should be invoked when the actor becomes
    /// idle after performing an action.
    fn activate_on_idle(&self) -> bool {
        false
    }

    /// Activates an actor.
    ///
    /// Actors are responsible for scheduling their own activation based on any of the other
    /// triggers in this trait. The only time `on_active()` is called automatically is when
    /// `activate_on_idle()` returns `true` and the actor becomes idle again after performing
    /// some other action.
    fn on_active(&self, _realm: &mut Realm, _dispatcher: &ActionDispatcher) -> ActionOutput {
        Ok(Vec::new())
    }

    /// Invoked when the actor itself is being attacked.
    fn on_attack(
        &self,
        _realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        _attacker: EntityRef,
    ) -> ActionOutput {
        Ok(Vec::new())
    }

    /// Invoked when a character in the same room is being attacked.
    fn on_character_attacked(
        &self,
        _realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        _attacker: EntityRef,
        _defendent: EntityRef,
    ) -> ActionOutput {
        Ok(Vec::new())
    }

    /// Invoked when a character in the same room dies.
    ///
    /// If the character dies as a direct result of another character's attack, the attacker
    /// is given as well.
    fn on_character_died(
        &self,
        _realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        _casualty: EntityRef,
        _attacker: Option<EntityRef>,
    ) -> ActionOutput {
        Ok(Vec::new())
    }

    /// Invoked when a character entered the same room.
    fn on_character_entered(
        &mut self,
        _realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        _character: EntityRef,
    ) -> ActionOutput {
        Ok(Vec::new())
    }

    /// Invoked when the character itself dies.
    ///
    /// If the character dies as a direct result of another character's attack, the attacker
    /// is given as well.
    fn on_die(
        &self,
        _realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        _attacker: Option<EntityRef>,
    ) -> ActionOutput {
        Ok(Vec::new())
    }

    /// Invoked when the actor is spawned.
    fn on_spawn(&self, _realm: &mut Realm, _dispatcher: &ActionDispatcher) -> ActionOutput {
        Ok(Vec::new())
    }

    /// Invoked when a character talks directly to the actor.
    fn on_talk(
        &self,
        _realm: &mut Realm,
        _dispatcher: &ActionDispatcher,
        _speaker: EntityRef,
        _message: &str,
    ) -> ActionOutput {
        Ok(Vec::new())
    }
}
