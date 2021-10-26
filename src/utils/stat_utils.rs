use crate::{
    character_stats::{CharacterStat, CharacterStats},
    entity::{Character, EntityRef, Realm},
};

/// Returns the weight of all the inventory (including those currently wielded) carried by the
/// character.
fn inventory_weight(realm: &Realm, character: &Character) -> f32 {
    let inventory_weight = character
        .inventory()
        .iter()
        .filter_map(|item| realm.item(*item))
        .map(|item| item.weight())
        .sum();

    /* TODO
    if (this.weapon) {
        inventoryWeight += this.weapon.weight;
    }
    if (this.secondaryWeapon) {
        inventoryWeight += this.secondaryWeapon.weight;
    }
    if (this.shield) {
        inventoryWeight += this.shield.weight;
    }*/
    inventory_weight
}

/**
 * Returns the character's stats, including effects from modifiers and equipment wielded.
 */
pub fn total_stats(realm: &Realm, entity_ref: EntityRef) -> CharacterStats {
    let mut total_stats = realm
        .stats_item(entity_ref)
        .map(|stats_item| {
            let stats = stats_item.stats().clone();

            /* TODO
            if (this.modifiers) {
                this.modifiers.forEach(function(modifier) {
                    totalStats.add(modifier.stats);
                });
            }*/

            stats
        })
        .unwrap_or_default();

    if let Some(character) = realm.character(entity_ref) {
        /* TODO
        if (this.weapon) {
            totalStats.add(this.weapon.totalStats());
        }*/
        /* TODO
        if (this.secondaryWeapon) {
            totalStats.add(this.secondaryWeapon.totalStats());
        }*/
        /* TODO
        if (this.shield) {
            totalStats.add(this.shield.totalStats());
        }*/

        let dexterity = total_stats.get(CharacterStat::Dexterity)
            - (inventory_weight(realm, character) / 5.0).floor() as i16;
        total_stats.set(CharacterStat::Dexterity, dexterity.max(0));
    }

    total_stats
}
