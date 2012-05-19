#include "admincommand.h"


AdminCommand::AdminCommand(Player *character, QObject *parent) :
    Command(character, parent) {
}

AdminCommand::~AdminCommand() {
}

GameObjectPtrList AdminCommand::objectsByDescription(const QPair<QString, uint> &description,
                                                     const GameObjectPtrList &pool) {

    if (description.first == "area") {
        GameObjectPtrList objects;
        objects << currentArea();
        return objects;
    }

    return Command::objectsByDescription(description, pool);
}
