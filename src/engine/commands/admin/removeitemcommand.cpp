#include "removeitemcommand.h"


#define super AdminCommand

RemoveItemCommand::RemoveItemCommand(QObject *parent) :
    super(parent) {

    setDescription("Remove an item from the current area.\n"
                   "\n"
                   "Usage: remove-item <item-name> [#]");
}

RemoveItemCommand::~RemoveItemCommand() {
}

void RemoveItemCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);
    if (!assertWordsLeft("Usage: remove-item <item-name> [#]")) {
        return;
    }

    GameObjectPtrList items = takeObjects(currentRoom()->items());
    if (!requireSome(items, "That's not here.")) {
        return;
    }

    for (const GameObjectPtr &item : items) {
        item->setDeleted();

        send(QString("Item %1 removed.").arg(item->name()));
    }
}
