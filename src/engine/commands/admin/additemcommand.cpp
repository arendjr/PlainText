#include "additemcommand.h"

#include "item.h"


#define super AdminCommand

AddItemCommand::AddItemCommand(QObject *parent) :
    super(parent) {

    setDescription("Add an item to the current area.\n"
                   "\n"
                   "Usage: add-item <item-name>");
}

AddItemCommand::~AddItemCommand() {
}

void AddItemCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: add-item <item-name>")) {
        return;
    }

    QString itemName = takeWord();

    Item *item = GameObject::createByObjectType<Item *>(realm(), "item");
    item->setName(itemName);
    currentRoom()->addItem(item);

    send(QString("Item %1 added.").arg(itemName));
}
