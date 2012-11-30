#ifndef TEST_MOVEMENT_H
#define TEST_MOVEMENT_H

#include "testcase.h"

#include <QTest>

#include "player.h"
#include "portal.h"
#include "realm.h"
#include "room.h"


class MovementTest : public TestCase {

    Q_OBJECT

    private slots:
        void testMovement() {

            Realm *realm = Realm::instance();
            Player *player = (Player *) realm->getPlayer("Arie");
            Portal *portal = (Portal *) realm->getObject(GameObjectType::Portal, 3);

            QCOMPARE(player->currentRoom()->name(), QString("Room A"));

            {
                player->go(portal);
                QCOMPARE(player->currentRoom()->name(), QString("Room B"));
            }

            {
                player->go(portal);
                QCOMPARE(player->currentRoom()->name(), QString("Room A"));
            }

            {
                Room *currentRoom = player->currentRoom().cast<Room *>();
                player->go(currentRoom->exits()[0]);
                QCOMPARE(player->currentRoom()->name(), QString("Room B"));
            }

            {
                player->execute("go b-to-a");
                QCOMPARE(player->currentRoom()->name(), QString("Room A"));
            }

            {
                player->execute("a-to-b");
                QCOMPARE(player->currentRoom()->name(), QString("Room B"));
            }

            {
                player->go(realm->getObject(GameObjectType::Room, 1));
                QCOMPARE(player->currentRoom()->name(), QString("Room A"));
            }
        }
};

#endif // TEST_MOVEMENT_H
