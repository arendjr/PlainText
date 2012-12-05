#include "triggerregistry.h"

#include <QStringList>


TriggerRegistry::TriggerRegistry(QObject *parent) :
    QObject(parent) {

    m_triggers.insert("onactive : void",
                      "The onactive trigger is invoked on any character when it's no longer "
                      "stunned, ie. when it can perform a new action again.");
    m_triggers.insert("onattack(attacker : character) : bool",
                      "The onattack trigger is invoked on any character when it's being "
                      "attacked.");
    m_triggers.insert("onbuy(buyer : character, boughtItem : optional item) : bool",
                      "The onbuy trigger is invoked on any character when something is being "
                      "bought from it. When boughtItem is omitted, the buyer is requesting an "
                      "overview of the things for sale.");
    m_triggers.insert("oncharacterattacked(attacker : character, defendant : character) : void",
                      "The oncharacterattacked trigger is invoked on any character in a room, "
                      "except for the attacker and defendant themselves, when another "
                      "character in that room emerges into combat.");
    m_triggers.insert("oncharacterdied(defendant : character, attacker : optional character) : "
                      "bool",
                      "The oncharacterdied trigger is invoked on any character in a room, when "
                      "another character in that room dies. When attacker is omitted, the "
                      "defendant died because of a non-combat cause (for example, poison).");
    m_triggers.insert("oncharacterentered(activator : character) : void",
                      "The oncharacterentered trigger is invoked on any character in a room when "
                      "another character enters that room.");
    m_triggers.insert("onclose(activator : character) : bool",
                      "The onclose trigger is invoked on any item or exit when it's closed.");
    m_triggers.insert("oncombat(attacker : character, defendant : character, observers : list, "
                      "damage : integer) : bool",
                      "This trigger is defined on the realm, and may be defined on individual "
                      "rooms. Its primary responsibility is generating the messages that are "
                      "displayed during combat, but it may also provide special combat effects. "
                      "If the room in which the combat takes place has this trigger defined, it "
                      "is used instead of the generic trigger from the realm. For this trigger, "
                      "the return value cannot be used to cancel the combat, but if a room's "
                      "trigger returns false it will fall back to the realm's trigger.");
    m_triggers.insert("ondie(attacker : optional character) : bool",
                      "The ondie trigger is invoked on any character when it dies. When "
                      "attacker is omitted, the character died because of a non-combat cause "
                      "(for example, poison).");
    m_triggers.insert("ondrink(activator : character) : bool",
                      "The ondrink trigger is invoked on any item when it's drunk.");
    m_triggers.insert("oneat(activator : character) : bool",
                      "The ondrink trigger is invoked on any item when it's eaten.");
    m_triggers.insert("onenter(activator : character) : bool",
                      "The onenter trigger is invoked on any exit when it's entered.");
    m_triggers.insert("onentered : void",
                      "The onentered trigger is invoked on any character when it entered a new "
                      "room.");
    m_triggers.insert("oncharacterexit(activator : character, exitName : string) : bool",
                      "The onexit trigger is invoked on any character in a room when another "
                      "character leaves that room.");
    m_triggers.insert("oninit : void",
                      "The oninit trigger is invoked once on every object when the game server "
                      "is started. Note: For characters that do have an onspawn trigger, but "
                      "no oninit trigger, onspawn is triggered instead.");
    m_triggers.insert("onopen(activator : character) : bool",
                      "The onopen trigger is invoked on any item or exit when it's opened.");
    m_triggers.insert("onreceive(giver : character, item : item or amount) : bool",
                      "The onreceive trigger is invoked on any character when something is "
                      "being given to it. Note that item may be a number instead of an item "
                      "object when an amount of gold is being given.");
    m_triggers.insert("onshout(activator : character, message : string) : void",
                      "The onshout trigger is invoked on any character when it hears someone "
                      "shout.");
    m_triggers.insert("onsound(message : string) : void",
                      "The onsound trigger is invoked when a sound event is perceived.");
    m_triggers.insert("onspawn : void",
                      "The onspawn trigger is invoked on any character when it respawns.");
    m_triggers.insert("ontalk(speaker : character, message : string) : void",
                      "The ontalk trigger is invoked on any character when talked to.");
    m_triggers.insert("onuse(activator : character) : void",
                      "The onuse trigger is invoked on any item when it's used.");
    m_triggers.insert("onvisual(message : string) : void",
                      "The onvisual trigger is invoked when a visual event is perceived.");
}

TriggerRegistry::~TriggerRegistry() {
}

void TriggerRegistry::registerTrigger(const QString &signature, const QString &description) {

    m_triggers.insert(signature, description);
}

const QMap<QString, QString> &TriggerRegistry::triggers() {

    return m_triggers;
}

QStringList TriggerRegistry::signatures() const {

    return m_triggers.keys();
}

QString TriggerRegistry::description(const QString &signature) const {

    return m_triggers[signature];
}
