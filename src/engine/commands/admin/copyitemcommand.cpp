#include "copyitemcommand.h"

#include "item.h"


#define super AdminCommand

CopyItemCommand::CopyItemCommand(QObject *parent) :
    super(parent) {

    setDescription("Copies an item or character.\n"
                   "\n"
                   "Usage: copy-item <item-name> [#]");
}

CopyItemCommand::~CopyItemCommand() {
}

void CopyItemCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: copy-item <item-name> [#]")) {
        return;
    }

    GameObjectPtr item = takeObject(currentRoom()->items() + currentRoom()->characters());
    if (!requireSome(item, "Item not found.")) {
        return;
    }

    GameObject *copy = item->copy();

    if (copy->isCharacter()) {
        currentRoom()->addCharacter(copy);

        send(QString("Character %1 copied.").arg(item->name()));
    } else {
        currentRoom()->addItem(copy);

        send(QString("Item %1 copied.").arg(item->name()));
    }
}
