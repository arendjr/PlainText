#include "usecommand.h"

#include "engine/util.h"


UseCommand::UseCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Use an item from the current area or your inventory.\n"
                   "\n"
                   "Example: use key");
}

UseCommand::~UseCommand() {
}

void UseCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Use what?")) {
        return;
    }

    takeWord("the");

    GameObjectPtrList allItems = player()->inventory() + currentArea()->items();
    GameObjectPtrList items = takeObjects(allItems);
    if (!requireSome(items, "That's not here.")) {
        return;
    }

    Item *item = items[0].cast<Item *>();
    if (!item->hasTrigger("onuse")) {
        send("You cannot use that.");
        return;
    }

    item->invokeTrigger("onuse", player());
}
