#include "dropcommand.h"


DropCommand::DropCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Drop an item or money from your inventory.\n"
                   "\n"
                   "Example: drop stick");
}

DropCommand::~DropCommand() {
}

void DropCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Drop what?")) {
        return;
    }

    QString description;
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
            send("You drop nothing.");
            return;
        }
        if (gold > player()->gold()) {
            send("You don't have that much gold.");
            return;
        }

        currentArea()->addGold(gold);
        player()->adjustGold(-gold);

        description = QString("$%1 worth of gold").arg(gold);
    } else {
        prependWord(word);
        takeWord("the");

        items = takeObjects(player()->inventory());
        if (!requireSome(items, "You don't have that.")) {
            return;
        }

        for (const GameObjectPtr &item : items) {
            currentArea()->addItem(item);
            player()->removeInventoryItem(item);
        }

        description = items.joinFancy(DefiniteArticles);
    }

    send(QString("You drop %2.").arg(description));

    GameObjectPtrList others = currentArea()->players();
    others.removeOne(player());
    others.send(QString("%1 drops %2.").arg(player()->name(), description));
}
