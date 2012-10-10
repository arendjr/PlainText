#include "admincommand.h"

#include "realm.h"


#define super Command

AdminCommand::AdminCommand(QObject *parent) :
    super(parent) {
}

AdminCommand::~AdminCommand() {
}

GameObjectPtrList AdminCommand::objectsByDescription(const QPair<QString, uint> &description,
                                                     const GameObjectPtrList &pool) {

    if (description.first == "room") {
        GameObjectPtrList objects;
        objects << currentRoom();
        return objects;
    }

    if (description.first.startsWith("#")) {
        uint objectId = description.first.mid(1).toInt();
        if (objectId > 0 || description.first == "#0") {
            GameObjectPtrList objects;
            if (description.second < 2) {
                objects << realm()->getObject(0, objectId);
            }
            return objects;
        }
    }

    return super::objectsByDescription(description, pool);
}
