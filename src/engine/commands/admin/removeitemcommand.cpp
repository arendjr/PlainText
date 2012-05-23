#include "removeitemcommand.h"


RemoveItemCommand::RemoveItemCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Remove an item from the current area.\n"
                   "\n"
                   "Usage: remove-item <item-name> [#]");
}

RemoveItemCommand::~RemoveItemCommand() {
}

void RemoveItemCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: remove-item <item-name> [#]")) {
        return;
    }

    GameObjectPtrList items = takeObjects(currentArea()->items());
    if (!requireSome(items, "That's not here.")) {
        return;
    }

    foreach (const GameObjectPtr &item, items) {
        currentArea()->removeItem(item);

        player()->send(QString("Item %1 removed.").arg(item->name()));

        item->setDeleted();
    }
}
