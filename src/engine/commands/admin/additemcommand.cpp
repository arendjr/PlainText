#include "additemcommand.h"

#include "item.h"


#define super AdminCommand

AddItemCommand::AddItemCommand(QObject *parent) :
    super(parent) {

    setDescription("Add an item to the current room.\n"
                   "\n"
                   "Usage: add-item <item-name>");
}

AddItemCommand::~AddItemCommand() {
}

void AddItemCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: add-item <item-name>")) {
        return;
    }

    QString itemName = takeWord();

    Item *item = new Item(realm());
    item->setName(itemName);
    currentRoom()->addItem(item);

    send("Item %1 added.", itemName);
}
