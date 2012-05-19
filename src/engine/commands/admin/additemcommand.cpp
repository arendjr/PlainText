#include "additemcommand.h"

#include "engine/item.h"
#include "engine/util.h"


AddItemCommand::AddItemCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {
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

    Item *item = qobject_cast<Item *>(GameObject::createByObjectType("item"));
    item->setName(itemName);
    currentArea()->addItem(item);

    player()->send(QString("Item %1 added.").arg(itemName));
}
