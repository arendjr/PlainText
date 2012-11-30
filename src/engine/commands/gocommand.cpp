#include "gocommand.h"

#include "portal.h"


#define super Command

GoCommand::GoCommand(QObject *parent) :
    super(parent) {

    setDescription("Go to or enter an exit.\n"
                   "\n"
                   "Examples: go north, go to tower, enter door");
}

GoCommand::~GoCommand() {
}

void GoCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (alias() == "go") {
        takeWord("to", IfNotLast);
    }

    ObjectDescription description = takeObjectsDescription();
    GameObjectPtrList portals;
    for (const GameObjectPtr &portalPtr : currentRoom()->portals()) {
        Portal *portal = portalPtr.cast<Portal *>();
        QString loweredName = portal->nameFromRoom(player->currentRoom()).toLower();
        for (const QString &word : loweredName.split(' ')) {
            if (word.startsWith(description.name)) {
                portals.append(portal);
                break;
            }
        }
    }
    if (description.position > 0) {
        if (description.position <= (int) portals.length()) {
            GameObjectPtr selected = portals[description.position - 1];
            portals.clear();
            portals.append(selected);
        } else {
            portals.clear();
        }
    }

    if (!requireSome(portals, "Go where?")) {
        return;
    }

    player->go(portals[0]);
}
