#include "admincommand.h"

#include "realm.h"


#define super Command

AdminCommand::AdminCommand(QObject *parent) :
    super(parent) {
}

AdminCommand::~AdminCommand() {
}

GameObjectPtrList AdminCommand::objectsByDescription(const ObjectDescription &description,
                                                     const GameObjectPtrList &pool) {

    if (description.name == "room") {
        GameObjectPtrList objects;
        objects << currentRoom();
        return objects;
    }

    if (description.name.startsWith("#")) {
        uint objectId = description.name.mid(1).toInt();
        if (objectId > 0 || description.name == "#0") {
            GameObjectPtrList objects;
            if (description.position < 2) {
                objects << realm()->getObject(GameObjectType::Unknown, objectId);
            }
            return objects;
        }
    }

    return super::objectsByDescription(description, pool);
}
