use crate::game_object::{Character, GameObject, GameObjectRef};
use crate::objects::Realm;
use crate::player_output::PlayerOutput;
use crate::text_utils::{describe_items, format_weight, join_sentence};

use super::CommandHelpers;

/// Makes the character inspect their inventory.
///
/// Examples: `inventory`, `look in inventory`.
pub fn inventory(
    realm: Realm,
    player_ref: GameObjectRef,
    helpers: CommandHelpers,
) -> (Realm, Vec<PlayerOutput>) {
    let processor = helpers.command_line_processor;

    let mut output = Vec::new();
    let player = unwrap_or_return!(realm.player(player_ref), (realm, output));

    processor.skip_word(); // alias

    let inventory = player.inventory();
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
            join_sentence(describe_items(&realm, inventory))
        )
    } else if let Some(item) =
        GameObjectRef::only(inventory).and_then(|item_ref| realm.item(item_ref))
    {
        format!(
            "You carry {}, weighing {}.\n",
            item.indefinite_name(),
            format_weight(weight)
        )
    } else {
        format!(
            "You carry {}, weighing a total of {}.\n",
            join_sentence(describe_items(&realm, inventory)),
            format_weight(weight)
        )
    };

    let carried_gold_string = if player.gold() == 0 {
        "You don't have any gold.\n".to_owned()
    } else {
        format!("You've got ${} worth of gold.\n", player.gold())
    };

    push_output_string!(
        output,
        player_ref,
        carried_inventory_description + &carried_gold_string
    );

    (realm, output)
}
