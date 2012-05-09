#include "removeitemcommand.h"


RemoveItemCommand::RemoveItemCommand(Character *character, QObject *parent) :
    AdminCommand(character, parent) {
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

        character()->send(QString("Item %1 removed.").arg(item->name()));

        item->deleteLater();
    }
}
