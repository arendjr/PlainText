#include "dropcommand.h"


#define super Command

DropCommand::DropCommand(QObject *parent) :
    super(parent) {

    setDescription("Drop an item or money from your inventory.\n"
                   "\n"
                   "Example: drop stick");
}

DropCommand::~DropCommand() {
}

void DropCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    QString description;
    GameObjectPtrList items;
    double gold = 0.0;

    QString word = takeWord();
    if (word.startsWith("$")) {
        QRegExp currencyRegExp("\\$\\d+(\\.5)?");
        if (!currencyRegExp.exactMatch(word)) {
            send("%1 is not a valid currency description.", word);
            return;
        }
        gold = word.mid(1).toDouble();
        if (gold == 0.0) {
            send("You drop nothing.");
            return;
        }
        if (gold > player->gold()) {
            send("You don't have that much gold.");
            return;
        }

        currentRoom()->addGold(gold);
        player->adjustGold(-gold);

        description = QString("$%1 worth of gold").arg(gold);
    } else {
        prependWord(word);

        items = takeObjects(player->inventory());
        if (!requireSome(items, "Drop what?")) {
            return;
        }

        for (const GameObjectPtr &item : items) {
            currentRoom()->addItem(item);
            player->removeInventoryItem(item);
        }

        description = items.joinFancy(DefiniteArticles);
    }

    send("You drop %2.", description);

    GameObjectPtrList others = currentRoom()->characters();
    others.removeOne(player);
    others.send(QString("%1 drops %2.").arg(player->name(), description));
}
