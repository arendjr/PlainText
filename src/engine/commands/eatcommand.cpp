#include "eatcommand.h"

#include "engine/util.h"


EatCommand::EatCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Eat an item in the current area or your inventory.\n"
                   "\n"
                   "Example: eat cake");
}

EatCommand::~EatCommand() {
}

void EatCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Eat what?")) {
        return;
    }

    takeWord("the");

    GameObjectPtrList allItems = player()->inventory() + currentArea()->items();
    GameObjectPtrList items = takeObjects(allItems);
    if (!requireSome(items, "That's not here.")) {
        return;
    }

    Item *item = items[0].cast<Item *>();
    if (!item->hasTrigger("oneat")) {
        send("You cannot eat that.");
        return;
    }

    if (!item->invokeTrigger("oneat", player())) {
        return;
    }

    item->setDeleted();
}
