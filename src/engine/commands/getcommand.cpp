#include "getcommand.h"

#include "container.h"
#include "util.h"


#define super Command

GetCommand::GetCommand(QObject *parent) :
    super(parent) {

    setDescription("Take an item or gold from the current area or a container and put it in your "
                   "inventory.\n"
                   "\n"
                   "Examples: get stick, take stick, get gold, get stick from bag");
}

GetCommand::~GetCommand() {
}

void GetCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    ObjectDescription description = takeObjectsDescription();

    if (!takeWord("from").isEmpty()) {
        GameObjectPtr containerPtr = takeObject(player->inventory());
        if (!requireSome(containerPtr, QString("%1 from what?").arg(Util::capitalize(alias())))) {
            return;
        }
        if (!containerPtr->isContainer()) {
            send("%1 is not a container", containerPtr->definiteName(player->inventory(),
                                                                     Capitalized));
            return;
        }

        Container *container = containerPtr.cast<Container *>();
        GameObjectPtrList items = objectsByDescription(description, container->items());
        if (!requireSome(items, QString("%1 what?").arg(Util::capitalize(alias())))) {
            return;
        }

        for (const GameObjectPtr &item : items) {
            player->addInventoryItem(item);
            container->removeItem(item);
        }

        send("You take %1 from %2.", items.joinFancy(DefiniteArticles),
             container->definiteName(player->inventory()));
    } else {
        GameObjectPtrList items = objectsByDescription(description, currentRoom()->items());
        if (!requireSome(items, QString("%1 what?").arg(Util::capitalize(alias())))) {
            return;
        }

        player->take(items);
    }
}
