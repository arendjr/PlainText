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

    takeWord("the");

    GameObjectPtrList items = takeObjects(player()->inventory());
    if (!requireSome(items, "You don't have that.")) {
        return;
    }

    for (const GameObjectPtr &item : items) {
        currentArea()->addItem(item);
        player()->removeInventoryItem(item);
    }

    QString description = items.joinFancy(DefiniteArticles);
    send(QString("You drop %2.").arg(description));

    GameObjectPtrList others = currentArea()->players();
    others.removeOne(player());
    others.send(QString("%1 drops %2.").arg(player()->name(), description));
}
