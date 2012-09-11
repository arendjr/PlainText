#include "copyitemcommand.h"

#include "item.h"
#include "util.h"


CopyItemCommand::CopyItemCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Copies an item or character.\n"
                   "\n"
                   "Usage: copy-item <item-name> [#]");
}

CopyItemCommand::~CopyItemCommand() {
}

void CopyItemCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: copy-item <item-name> [#]")) {
        return;
    }

    GameObjectPtr item = takeObject(currentArea()->items() + currentArea()->npcs());
    if (!requireSome(item, "Item not found.")) {
        return;
    }

    GameObject *copy = item->copy();

    if (copy->isCharacter()) {
        currentArea()->addNPC(copy);

        send(QString("Character %1 copied.").arg(item->name()));
    } else {
        currentArea()->addItem(copy);

        send(QString("Item %1 copied.").arg(item->name()));
    }
}
