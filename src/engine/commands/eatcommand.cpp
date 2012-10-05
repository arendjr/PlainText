#include "eatcommand.h"

#include "util.h"


#define super Command

EatCommand::EatCommand(QObject *parent) :
    super(parent) {

    setDescription("Eat an item in the current area or your inventory.\n"
                   "\n"
                   "Example: eat cake");
}

EatCommand::~EatCommand() {
}

void EatCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Eat what?")) {
        return;
    }

    takeWord("the");

    GameObjectPtrList allItems = player->inventory() + currentArea()->items();
    GameObjectPtr item = takeObject(allItems);
    if (!requireSome(item, "You don't have that.")) {
        return;
    }

    if (!item->hasTrigger("oneat")) {
        send("You cannot eat that.");
        return;
    }

    if (!item->invokeTrigger("oneat", player)) {
        return;
    }

    item->setDeleted();
}
