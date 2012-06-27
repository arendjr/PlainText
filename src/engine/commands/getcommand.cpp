#include "getcommand.h"

#include "engine/util.h"


GetCommand::GetCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Take an item or gold from the current area and put it in "
                   "your inventory.\n"
                   "\n"
                   "Examples: get stick, take stick, get gold");
}

GetCommand::~GetCommand() {
}

void GetCommand::execute(const QString &command) {

    setCommand(command);

    QString alias = takeWord();
    if (!assertWordsLeft(QString("%1 what?").arg(Util::capitalize(alias)))) {
        return;
    }

    takeWord("the");

    GameObjectPtrList allItems = currentArea()->items();
    GameObjectPtrList items = takeObjects(allItems);
    if (!requireSome(items, "That's not here.")) {
        return;
    }

    GameObjectPtrList takenItems;
    foreach (const GameObjectPtr &itemPtr, items) {
        Item *item = itemPtr.cast<Item *>();
        if (item->isPortable()) {
            player()->addInventoryItem(itemPtr);
            currentArea()->removeItem(itemPtr);
            takenItems << itemPtr;
        } else {
            player()->send(QString("You can't take %2.").arg(item->definiteName(allItems)));
        }
    }

    if (takenItems.length() > 0) {
        QString itemsDescription = Util::joinItems(takenItems, DefiniteArticles);
        player()->send(QString("You %1 %2.").arg(alias, itemsDescription));

        Util::sendOthers(currentArea()->characters(),
                         QString("%1 %2s %3.").arg(player()->name(), alias, itemsDescription),
                         player());
    }
}
