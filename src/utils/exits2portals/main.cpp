#include <QString>

#include "exit.h"
#include "portal.h"
#include "realm.h"
#include "room.h"

int main(int argc, char *argv[]) {

    Q_UNUSED(argc)
    Q_UNUSED(argv)

    Realm realm;
    realm.init();

    // make sure all exits have their opposite exit properly set
    for (const GameObjectPtr &exitPtr : realm.allObjects(GameObjectType::Exit)) {
        Exit *exit = exitPtr.cast<Exit *>();
        if (exit->oppositeExit().isNull()) {
            Room *destination = exit->destination().cast<Room *>();
            for (const GameObjectPtr &oppositeExitPtr : destination->exits()) {
                Exit *oppositeExit = oppositeExitPtr.cast<Exit *>();
                Room *source = oppositeExit->destination().cast<Room *>();
                if (source->exits().contains(exit)) {
                    exit->setOppositeExit(oppositeExitPtr);
                    oppositeExit->setOppositeExit(exitPtr);
                    break;
                }
            }
        }
    }

    // convert exits to portals
    for (const GameObjectPtr &roomPtr : realm.allObjects(GameObjectType::Room)) {
        Room *room = roomPtr.cast<Room *>();
        GameObjectPtrList exits = room->exits();
        for (const GameObjectPtr &exitPtr : exits) {
            Exit *exit = exitPtr.cast<Exit *>();

            Portal *portal = new Portal(&realm);
            portal->setRoom(roomPtr);
            portal->setName(exit->name());
            portal->setDescription(exit->description());

            room->removeExit(exitPtr);
            room->addPortal(portal);

            if (!exit->oppositeExit().isNull()) {
                Room *destination = exit->destination().cast<Room *>();

                Exit *oppositeExit = exit->oppositeExit().cast<Exit *>();
                portal->setRoom2(destination);
                portal->setName2(oppositeExit->name());
                portal->setDescription2(oppositeExit->description());

                destination->removeExit(oppositeExit);
                destination->addPortal(portal);

                oppositeExit->setDeleted();
            }

            exit->setDeleted();
        }
    }

    // save all
    for (const GameObjectPtr &exitPtr : realm.allObjects(GameObjectType::Exit)) {
        exitPtr->save();
    }
    for (const GameObjectPtr &portalPtr : realm.allObjects(GameObjectType::Portal)) {
        portalPtr->save();
    }
    for (const GameObjectPtr &roomPtr : realm.allObjects(GameObjectType::Room)) {
        roomPtr->save();
    }
}
