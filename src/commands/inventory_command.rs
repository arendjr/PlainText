use super::CommandHelpers;
use crate::{
    actions::ActionOutput,
    entity::{Entity, EntityRef, Realm},
    player_output::PlayerOutput,
    utils::{describe_items, format_weight, join_sentence},
};

/// Inspects the character's inventory.
pub fn inventory(
    realm: &mut Realm,
    player_ref: EntityRef,
    helpers: CommandHelpers,
) -> ActionOutput {
    let player = realm.player_res(player_ref)?;

    let processor = helpers.command_line_processor;
    processor.skip_word(); // alias

    let inventory = player.character.inventory();
    let weight = inventory.iter().fold(0.0, |weight, &item_ref| {
        weight
            + if let Some(item) = realm.item(item_ref) {
                item.weight()
            } else {
                0.0
            }
    });
    let carried_inventory_description = if inventory.is_empty() {
        "You don't carry anything.\n".to_owned()
    } else if weight.round() == 0.0 {
        format!(
            "You carry {}.\n",
            join_sentence(&describe_items(realm, inventory))
        )
    } else if let Some(item) = EntityRef::only(inventory).and_then(|item_ref| realm.item(item_ref))
    {
        format!(
            "You carry {}, weighing {}.\n",
            item.indefinite_name(),
            format_weight(weight)
        )
    } else {
        format!(
            "You carry {}, weighing a total of {}.\n",
            join_sentence(&describe_items(realm, inventory)),
            format_weight(weight)
        )
    };

    let gold = player.character.gold();
    let carried_gold_string = if gold == 0 {
        "You don't have any gold.\n".to_owned()
    } else {
        format!("You've got ${} worth of gold.\n", gold)
    };

    Ok(vec![PlayerOutput::new_from_string(
        player.id(),
        carried_inventory_description + &carried_gold_string,
    )])
}
