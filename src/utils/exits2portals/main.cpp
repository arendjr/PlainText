#include <QCoreApplication>
#include <QScriptEngine>
#include <QString>

#include "exit.h"
#include "metatyperegistry.h"
#include "portal.h"
#include "realm.h"
#include "room.h"


int main(int argc, char *argv[]) {

    QCoreApplication application(argc, argv);
    QScriptEngine jsEngine;
    MetaTypeRegistry::registerMetaTypes(&jsEngine);

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
            portal->setTriggers(exit->triggers());

            PortalFlags flags = PortalFlags::CanPassThrough;
            if (exit->isHidden()) {
                flags |= PortalFlags::IsHiddenFromSide1;
            }
            if (exit->isDoor()) {
                flags ^= PortalFlags::CanPassThrough;
                flags |= PortalFlags::CanPassThroughIfOpen;
                flags |= PortalFlags::CanOpenFromSide1;
            }

            room->removeExit(exitPtr);
            room->addPortal(portal);

            if (!exit->oppositeExit().isNull()) {
                Room *destination = exit->destination().cast<Room *>();

                Exit *oppositeExit = exit->oppositeExit().cast<Exit *>();
                portal->setRoom2(destination);
                portal->setName2(oppositeExit->name());
                portal->setDescription2(oppositeExit->description());
                for (const QString &triggerName : oppositeExit->triggers().keys()) {
                    portal->setTrigger(triggerName, oppositeExit->trigger(triggerName));
                }

                if (oppositeExit->isHidden()) {
                    flags |= PortalFlags::IsHiddenFromSide2;
                }
                if (oppositeExit->isDoor()) {
                    if (!exit->isDoor()) {
                        flags ^= PortalFlags::CanPassThrough;
                        flags |= PortalFlags::CanPassThroughIfOpen;
                    }
                    flags |= PortalFlags::CanOpenFromSide2;
                }

                destination->removeExit(oppositeExit);
                destination->addPortal(portal);

                oppositeExit->setDeleted();
            }

            portal->setFlags(flags);

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

    return 0;
}
