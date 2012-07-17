#include "drinkcommand.h"

#include "engine/util.h"


DrinkCommand::DrinkCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Drink an item in the current area or your inventory.\n"
                   "\n"
                   "Example: drink potion");
}

DrinkCommand::~DrinkCommand() {
}

void DrinkCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Drink what?")) {
        return;
    }

    takeWord("the");

    GameObjectPtrList allItems = player()->inventory() + currentArea()->items();
    GameObjectPtrList items = takeObjects(allItems);
    if (!requireSome(items, "You don't have that.")) {
        return;
    }

    Item *item = items[0].cast<Item *>();
    if (!item->hasTrigger("ondrink")) {
        send("You cannot drink that.");
        return;
    }

    if (!item->invokeTrigger("ondrink", player())) {
        return;
    }

    item->setDeleted();
}
