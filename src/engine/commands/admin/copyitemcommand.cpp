#include "copyitemcommand.h"

#include <cstring>

#include "engine/item.h"
#include "engine/util.h"


CopyItemCommand::CopyItemCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Copies an item, or character, in the current area.\n"
                   "\n"
                   "Usage: copy-item <item-name>");
}

CopyItemCommand::~CopyItemCommand() {
}

void CopyItemCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: copy-item <item-name>")) {
        return;
    }

    GameObjectPtrList items = takeObjects(currentArea()->items() + currentArea()->characters());
    if (!requireUnique(items, "Item not found.", "Item is not unique.")) {
        return;
    }

    Item *item = items[0].cast<Item *>();
    if (strcmp(item->objectType(), "player") == 0) {
        player()->send("Players may not be copied.");
        return;
    }

    GameObject *copy = item->copy();

    if (strcmp(item->objectType(), "item") == 0) {
        currentArea()->addItem(copy);

        player()->send(QString("Item %1 copied.").arg(item->name()));
    } else if (strcmp(item->objectType(), "character") == 0) {
        currentArea()->addNPC(copy);

        player()->send(QString("Character %1 copied.").arg(item->name()));
    }
}
