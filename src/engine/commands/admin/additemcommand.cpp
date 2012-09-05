#include "additemcommand.h"

#include "item.h"
#include "util.h"


AddItemCommand::AddItemCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Add an item to the current area.\n"
                   "\n"
                   "Usage: add-item <item-name>");
}

AddItemCommand::~AddItemCommand() {
}

void AddItemCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: add-item <item-name>")) {
        return;
    }

    QString itemName = takeWord();

    Item *item = GameObject::createByObjectType<Item *>(realm(), "item");
    item->setName(itemName);
    currentArea()->addItem(item);

    send(QString("Item %1 added.").arg(itemName));
}
