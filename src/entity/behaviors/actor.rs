use crate::entity::{Entity, EntityRef, Realm};

pub trait Actor: Entity {
    /// Invoked when the actor itself is being attacked.
    fn on_attack(&self, realm: &mut Realm, attacker: EntityRef);

    /// Invoked when a character in the same room is being attacked.
    fn on_character_attacked(&self, realm: &mut Realm, attacker: EntityRef, defendent: EntityRef);

    /// Invoked when a character in the same room dies.
    ///
    /// If the character dies as a direct result of another character's attack, the attacker
    /// is given as well.
    fn on_character_died(
        &self,
        realm: &mut Realm,
        casualty: EntityRef,
        attacker: Option<EntityRef>,
    );

    /// Invoked when a character entered the same room.
    fn on_character_entered(&self, realm: &mut Realm, character: EntityRef);

    /// Invoked when the actor is spawned.
    fn on_spawn(&self, realm: &mut Realm);

    /// Invoked when a character talks directly to the actor.
    fn on_talk(&self, realm: &mut Realm, talker: EntityRef, message: &str);
}
