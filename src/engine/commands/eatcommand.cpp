#include "eatcommand.h"

#include "util.h"


#define super Command

EatCommand::EatCommand(QObject *parent) :
    super(parent) {

    setDescription("Eat an item in the current room or your inventory.\n"
                   "\n"
                   "Example: eat cake");
}

EatCommand::~EatCommand() {
}

void EatCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr item = takeObject(player->inventory() + currentRoom()->items());
    if (!requireSome(item, "Eat what?")) {
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
