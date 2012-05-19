#include "getcommand.h"

#include "engine/character.h"
#include "engine/item.h"
#include "engine/util.h"


GetCommand::GetCommand(Player *character, QObject *parent) :
    Command(character, parent) {
}

GetCommand::~GetCommand() {
}

void GetCommand::execute(const QString &command) {

    setCommand(command);

    QString alias = takeWord();
    if (!assertWordsLeft("Get what?")) {
        return;
    }

    GameObjectPtrList items = takeObjects(currentArea()->items());
    if (!requireSome(items, "That's not here.")) {
        return;
    }

    GameObjectPtrList takenItems;
    foreach (const GameObjectPtr &item, items) {
        if (item.cast<Item *>()->isPortable()) {
            player()->addInventoryItem(item);
            currentArea()->removeItem(item);
            takenItems << item;
        } else {
            player()->send(QString("You can't take the %2.").arg(item->name()));
        }
    }

    if (takenItems.length() > 0) {
        QString itemsDescription = Util::joinItems(takenItems, DefiniteArticle);
        player()->send(QString("You %1 %2.").arg(alias, itemsDescription));

        Util::sendOthers(currentArea()->characters(),
                         QString("%1 %2s %3.").arg(player()->name(), alias, itemsDescription),
                         player());
    }
}
