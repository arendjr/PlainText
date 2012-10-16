#include "putcommand.h"

#include "container.h"
#include "util.h"


#define super Command

PutCommand::PutCommand(QObject *parent) :
    super(parent) {

    setDescription("Put an item from your inventory into a container.\n"
                   "\n"
                   "Example: put stick in bag");
}

PutCommand::~PutCommand() {
}

void PutCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtrList items = takeObjects(player->inventory());
    if (!requireSome(items, "Put what?")) {
        return;
    }

    takeWord("in(to)?");

    GameObjectPtr containerPtr = takeObject(player->inventory());
    if (!requireSome(containerPtr, "Put where?")) {
        return;
    }
    if (!containerPtr->isContainer()) {
        send("%1 is not a container", containerPtr->definiteName(player->inventory(), Capitalized));
        return;
    }

    Container *container = containerPtr.cast<Container *>();

    for (const GameObjectPtr &item : items) {
        container->addItem(item);
        player->removeInventoryItem(item);
    }

    send("You put %1 in %2.", items.joinFancy(DefiniteArticles),
         container->definiteName(player->inventory()));
}
