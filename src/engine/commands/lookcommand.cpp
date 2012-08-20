#include "lookcommand.h"

#include "engine/item.h"
#include "engine/race.h"
#include "engine/util.h"
#include "engine/weapon.h"


LookCommand::LookCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Look at something (any object, character, or the current "
                   "area).\n"
                   "\n"
                   "Examples: look, look door, look earl");
}

LookCommand::~LookCommand() {
}

void LookCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!hasWordsLeft()) {
        player()->look();
        return;
    }

    takeWord("at", IfNotLast);
    takeWord("the");

    if (!assertWordsLeft("Look at what?")) {
        return;
    }

    GameObjectPtrList pool = player()->inventory() + currentArea()->objects();
    GameObjectPtr object = takeObject(pool);
    if (!requireSome(object, "That's not here.")) {
        return;
    }

    QString description = object->description();

    if (player()->id() == object->id()) {
        QString m = "You look at yourself.\n";
        if (!description.isEmpty()) {
            m += description + "\n";
        }
        GameObjectPtrList wieldedItems;
        wieldedItems << player()->weapon()
                     << player()->secondaryWeapon()
                     << player()->shield();
        if (!wieldedItems.isEmpty()) {
            m += QString("You are wielding %1.\n").arg(wieldedItems.joinFancy());
        }
        send(m);
        return;
    }

    if (object->isCharacter()) {
        Character *character = object.cast<Character *>();

        QString m;
        if (character->isPlayer()) {
            m = QString("You look at %1, a %2 %3.\n")
                .arg(character->definiteName(pool),
                     character->race().cast<Race *>()->adjective(),
                     character->characterClass()->name());
        } else {
            m = QString("You look at %1.\n").arg(character->definiteName(pool));
        }

        if (!description.isEmpty()) {
            m += description + "\n";
        }

        GameObjectPtrList wieldedItems;
        wieldedItems << character->weapon()
                     << character->secondaryWeapon()
                     << character->shield();
        if (!wieldedItems.isEmpty()) {
            m += QString("%1 is wielding %2.\n")
                 .arg(Util::capitalize(character->subjectPronoun()))
                 .arg(wieldedItems.joinFancy());
        }

        int playerStatsTotal = player()->stats().total();
        if (!player()->weapon().isNull()) {
            playerStatsTotal += player()->weapon().cast<Weapon *>()->stats().total();
        }
        int characterStatsTotal = character->stats().total();
        if (!character->weapon().isNull()) {
            characterStatsTotal += character->weapon().cast<Weapon *>()->stats().total();
        }
        int statsDiff = playerStatsTotal - characterStatsTotal;

        if (statsDiff > 25) {
            if (player()->race()->name() == "giant" && character->race()->name() != "giant") {
                m += QString("You should be careful not to accidentally step on %1.\n")
                     .arg(character->objectPronoun());
            } else if (player()->race()->name() == "goblin" &&
                       character->race()->name() != "goblin") {
                m += QString("%1 should be thankful if you don't kill %2.\n")
                     .arg(Util::capitalize(character->subjectPronoun()),
                          character->objectPronoun());
            } else {
                m += QString("If it came to a fight with %1, %2'd better get %3 friends first.\n")
                     .arg(character->objectPronoun(), character->subjectPronoun(),
                          character->possessiveAdjective());
            }
        } else if (statsDiff > 15) {
            m += QString("%1 is not much of a match for you.\n")
                 .arg(Util::capitalize(character->subjectPronoun()));
        } else if (statsDiff > 5) {
            m += QString("You appear to be a bit stronger than %1.\n")
                 .arg(character->objectPronoun());
        } else if (statsDiff < -25) {
            m += QString("%1 could kill you with a needle.\n")
                 .arg(Util::capitalize(character->subjectPronoun()));
        } else if (statsDiff < -15) {
            m += QString("%1 appears like %2 could put up one hell of a fight.\n")
                 .arg(Util::capitalize(character->subjectPronoun()), character->subjectPronoun());
        } else if (statsDiff < -5) {
            m += QString("You should think twice before attacking %2.\n")
                 .arg(character->objectPronoun());
        } else {
            m += QString("%1 appears to be about as strong as you are.\n")
                 .arg(Util::capitalize(character->subjectPronoun()));
        }

        send(m);
    } else if (description.isEmpty()) {
        QString name;
        if (object->isItem()) {
            name = object.cast<Item *>()->definiteName(pool);
        } else {
            name = "the " + object->name();
        }
        send(QString("There's nothing special about %1.").arg(name));
    } else {
        send(description);
    }
}
