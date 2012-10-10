#include "givecommand.h"


#define super Command

GiveCommand::GiveCommand(QObject *parent) :
    super(parent) {

    setDescription("Give an item or gold from your inventory to another character.\n"
                   "\n"
                   "Examples: give stick earl, give $5 to beggar");
}

GiveCommand::~GiveCommand() {
}

void GiveCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Give what?")) {
        return;
    }

    GameObjectPtrList items;
    double gold = 0.0;

    QString word = takeWord();
    if (word.startsWith("$")) {
        QRegExp currencyRegExp("\\$\\d+(\\.5)?");
        if (!currencyRegExp.exactMatch(word)) {
            send(QString("%1 is not a valid currency description.").arg(word));
            return;
        }
        gold = word.mid(1).toDouble();
        if (gold == 0.0) {
            if (player->gender() == "male") {
                send("Be a gentleman, and give at least some gold.");
            } else {
                send("Be a lady, and give at least some gold.");
            }
            return;
        }
        if (gold > player->gold()) {
            send("You don't have that much gold.");
            return;
        }
    } else {
        prependWord(word);
        takeWord("the");

        items = takeObjects(player->inventory());
        if (!requireSome(items, "You don't have that.")) {
            return;
        }
    }

    takeWord("to", IfNotLast);
    takeWord("the", IfNotLast);

    GameObjectPtrList recipients = takeObjects(currentRoom()->characters());
    if (!requireSome(recipients, "That recipient is not here.")) {
        return;
    }

    Character *recipient = recipients[0].cast<Character *>();
    QString recipientName = recipient->definiteName(currentRoom()->characters());

    QString description;

    if (gold > 0.0) {
        if (!recipient->invokeTrigger("onreceive", player, gold)) {
            return;
        }

        recipient->adjustGold(gold);
        player->adjustGold(-gold);

        description = word;
    } else {
        GameObjectPtrList givenItems;
        for (const GameObjectPtr &itemPtr : items) {
            Item *item = itemPtr.cast<Item *>();
            if (recipient->inventoryWeight() + item->weight() > recipient->maxInventoryWeight()) {
                send(QString("%1 is too heavy for %2.")
                     .arg(item->definiteName(player->inventory(), Capitalized), recipientName));
            } else if (!recipient->invokeTrigger("onreceive", player, itemPtr)) {
                continue;
            } else {
                recipient->addInventoryItem(itemPtr);
                givenItems << itemPtr;
            }
        }

        description = givenItems.joinFancy();

        for (const GameObjectPtr &itemPtr : givenItems) {
            player->removeInventoryItem(itemPtr);
        }
    }

    send(QString("You give %1 to %2.").arg(description, recipientName));
    recipient->send(QString("%1 gives you %2.").arg(player->name(), description));

    GameObjectPtrList others = currentRoom()->players();
    others.removeOne(player);
    others.removeOne(recipient);
    others.send(QString("%1 gives %2 to %3.").arg(player->name(), description, recipientName));
}
