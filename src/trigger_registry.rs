use maplit::hashmap;
use std::collections::HashMap;
use strum::EnumDiscriminants;

use crate::game_object::GameObjectRef;

pub struct TriggerRegistry {
    triggers: HashMap<&'static str, &'static str>,
}

impl TriggerRegistry {
    pub fn new() -> Self {
        Self {
            triggers: create_triggers(),
        }
    }

    pub fn signatures(&self) -> Vec<String> {
        self.triggers.keys().map(|&k| k.to_owned()).collect()
    }
}

#[derive(Debug)]
enum ItemOrAmount {
    Gold(u32),
    Item(GameObjectRef),
}

#[derive(Debug, EnumDiscriminants)]
enum Trigger {
    Active,
    Attack {
        attacker_ref: GameObjectRef,
    },
    Buy {
        buyer_ref: GameObjectRef,
        bought_item: Option<GameObjectRef>,
    },
    CharacterAttacked {
        attacker: GameObjectRef,
        defendant: GameObjectRef,
    },
    CharacterDied {
        defendant: GameObjectRef,
        attacker: Option<GameObjectRef>,
    },
    CharacterEntered {
        activator: GameObjectRef,
    },
    CharacterExit {
        activator: GameObjectRef,
        exit_name: String,
    },
    Close {
        activator: GameObjectRef,
    },
    Combat {
        attacker: GameObjectRef,
        defendant: GameObjectRef,
        observers: Vec<GameObjectRef>,
        damage: i16,
    },
    Die {
        attacker: Option<GameObjectRef>,
    },
    Drink {
        activator: GameObjectRef,
    },
    Eat {
        activator: GameObjectRef,
    },
    Enter {
        activator: GameObjectRef,
    },
    Entered,
    Init,
    ItemDropped {
        item: GameObjectRef,
        owner: Option<GameObjectRef>,
    },
    Open {
        activator: GameObjectRef,
    },
    Receive {
        giver: GameObjectRef,
        item: ItemOrAmount,
    },
    Shout {
        activator: GameObjectRef,
        message: String,
    },
    Sound {
        message: String,
    },
    Spawn,
    Talk {
        speaker: GameObjectRef,
        message: String,
    },
    Use {
        activator: GameObjectRef,
    },
    Visual {
        message: String,
    },
}

fn create_triggers() -> HashMap<&'static str, &'static str> {
    hashmap! {
        "onActive: void" =>
            "The onActive trigger is invoked on any character when it's no longer stunned, ie. \
             when it can perform a new action again.",
        "onAttack(attacker: character): bool" =>
            "The onAttack trigger is invoked on any character when it's being attacked.",
        "onBuy(buyer: character, boughtItem?: item): bool" =>
            "The onBuy trigger is invoked on any character when something is being bought from it. \
             When boughtItem is omitted, the buyer is requesting an overview of the things for \
             sale.",
        "onCharacterAttacked(attacker: character, defendant: character): void" =>
            "The onCharacterAttacked trigger is invoked on any character in a room, except for the \
             attacker and defendant themselves, when another character in that room emerges into \
             combat.",
        "onCharacterDied(defendant: character, attacker?: character): bool" =>
            "The onCharacterDied trigger is invoked on any character in a room, when another \
             character in that room dies. When attacker is omitted, the defendant died because of \
             a non-combat cause (for example, poison).",
        "onCharacterEntered(activator: character): void" =>
            "The onCharacterEntered trigger is invoked on any character in a room when another \
             character enters that room.",
        "onCharacterExit(activator: character, exitName: string): bool" =>
            "The onCharacterExit trigger is invoked on any character in a room when another \
             character leaves that room.",
        "onClose(activator: character): bool" =>
            "The onClose trigger is invoked on any item or exit when it's closed.",
        "onCombat(attacker: character, defendant: character, observers: list, damage: integer): bool" =>
            "This trigger is defined on the realm, and may be defined on individual rooms. It \
             calculates how much damage (if any) is done and is responsible for generating the \
             messages that are displayed during combat, but it may also provide special combat \
             effects. If the room in which the combat takes place has this trigger defined, it is \
             used instead of the generic trigger from the realm. For this trigger, the return \
             value cannot be used to cancel the combat, but if a room's trigger returns false it \
             will fall back to the realm's trigger.",
        "onDie(attacker?: character): bool" =>
            "The onDie trigger is invoked on any character when it dies. When attacker is omitted, \
             the character died because of a non-combat cause (for example, poison).",
        "onDrink(activator: character): bool" =>
            "The onDrink trigger is invoked on any item when it's drunk.",
        "onEat(activator: character): bool" =>
            "The onEat trigger is invoked on any item when it's eaten.",
        "onEnter(activator: character): bool" =>
            "The onEnter trigger is invoked on any exit when it's entered.",
        "onEntered: void" =>
            "The onEntered trigger is invoked on any character when it entered a new room.",
        "onInit: void" =>
            "The onInit trigger is invoked once on every object when the game server is started.",
        "onItemDropped(item: item, owner?: character): void" =>
            "The onItemDropped trigger is invoked on all characters in a room when an item is \
             dropped in that room.",
        "onOpen(activator: character): bool" =>
            "The onOpen trigger is invoked on any item or exit when it's opened.",
        "onReceive(giver: character, item: item or amount): bool" =>
            "The onReceive trigger is invoked on any character when something is being given to \
             it. Note that item may be a number instead of an item object when an amount of gold \
             is being given.",
        "onShout(activator: character, message: string): void" =>
            "The onShout trigger is invoked on any character when it hears someone shout.",
        "onSound(message: string): void" =>
            "The onSound trigger is invoked when a sound event is perceived.",
        "onSpawn: void" =>
            "The onSpawn trigger is invoked on any character when it (re)spawns. Note: This is \
             also triggered right after onInit at a server start.",
        "onTalk(speaker: character, message: string): void" =>
            "The onTalk trigger is invoked on any character when talked to.",
        "onUse(activator: character): void" =>
            "The onUse trigger is invoked on any item when it's used.",
        "onVisual(message: string): void" =>
            "The onVisual trigger is invoked when a visual event is perceived."
    }
}
