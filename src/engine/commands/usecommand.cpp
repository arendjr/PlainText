#include "usecommand.h"

#include "util.h"


#define super Command

UseCommand::UseCommand(QObject *parent) :
    super(parent) {

    setDescription("Use an item from the current area or your inventory.\n"
                   "\n"
                   "Example: use key");
}

UseCommand::~UseCommand() {
}

void UseCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr item = takeObject(player->inventory() + currentRoom()->items());
    if (!requireSome(item, "Use what?")) {
        return;
    }

    if (!item->hasTrigger("onuse")) {
        send("You cannot use that.");
        return;
    }

    item->invokeTrigger("onuse", player);
}
