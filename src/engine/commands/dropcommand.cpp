#include "dropcommand.h"

#include "engine/util.h"


DropCommand::DropCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Drop an item or money from your inventory.\n"
                   "\n"
                   "Example: drop stick");
}

DropCommand::~DropCommand() {
}

void DropCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Drop what?")) {
        return;
    }

    takeWord("the");

    GameObjectPtrList items = takeObjects(player()->inventory());
    if (!requireSome(items, "You don't have that.")) {
        return;
    }

    gopl_foreach (item, items) {
        currentArea()->addItem(item);
        player()->removeInventoryItem(item);
    }

    QString itemsDescription = Util::joinItems(items, DefiniteArticles);
    player()->send(QString("You drop %2.").arg(itemsDescription));

    Util::sendOthers(currentArea()->characters(),
                     QString("%1 drops %2.").arg(player()->name(), itemsDescription),
                     player());
}
