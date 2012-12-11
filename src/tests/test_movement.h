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

                QCOMPARE(player->direction(), Vector3D(0, 100, 0));
            }

            {
                player->go(portal);
                QCOMPARE(player->currentRoom()->name(), QString("Room A"));

                QCOMPARE(player->direction(), Vector3D(0, -100, 0));
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

            {
                player->execute("enter a-to-b");
                QCOMPARE(player->currentRoom()->name(), QString("Room B"));
            }
        }

        void testMovementEvents() {

            Realm *realm = Realm::instance();
            Room *roomA = (Room *) realm->getObject(GameObjectType::Room, 1);
            Room *roomB = (Room *) realm->getObject(GameObjectType::Room, 2);
            Room *roomC = (Room *) realm->getObject(GameObjectType::Room, 5);

            Player *player = (Player *) realm->getPlayer("Arie");
            player->enter(roomA);

            Character *character = new Character(realm);
            character->setName("Boris");
            character->enter(roomC);

            evaluate("var sounds = [], visuals = [];");
            character->setTrigger("onsound", "function(message) { sounds.append(message); }");
            character->setTrigger("onvisual", "function(message) { visuals.append(message); }");

            // the character's looking toward room A, seeing all the action
            character->setDirection(roomA->position() - roomC->position());

            {
                player->go(roomB);

                QCOMPARE(evaluate("sounds.length").toInt32(), 0);
                QCOMPARE(evaluate("visuals.length").toInt32(), 1);
                QCOMPARE(evaluate("visuals[0]").toString(),
                         QString("You see Arie walking toward you."));
            }

            {
                player->go(roomC);

                QCOMPARE(evaluate("sounds.length").toInt32(), 0);
                QCOMPARE(evaluate("visuals.length").toInt32(), 2);
                QCOMPARE(evaluate("visuals[1]").toString(),
                         QString("Arie walks to you."));
            }

            {
                player->go(roomB);

                QCOMPARE(evaluate("sounds.length").toInt32(), 0);
                QCOMPARE(evaluate("visuals.length").toInt32(), 3);
                QCOMPARE(evaluate("visuals[2]").toString(),
                         QString("Arie walks to the c-to-b."));
            }

            {
                player->go(roomA);

                QCOMPARE(evaluate("sounds.length").toInt32(), 0);
                QCOMPARE(evaluate("visuals.length").toInt32(), 4);
                QCOMPARE(evaluate("visuals[3]").toString(),
                         QString("You see Arie walking north."));
            }

            // the character's looking away from room A, still hearing most of the action
            character->setDirection(roomC->position() - roomA->position());

            {
                player->go(roomB);

                QCOMPARE(evaluate("sounds.length").toInt32(), 0);
                QCOMPARE(evaluate("visuals.length").toInt32(), 4);
            }

            {
                player->go(roomC);

                QCOMPARE(evaluate("sounds.length").toInt32(), 1);
                QCOMPARE(evaluate("visuals.length").toInt32(), 4);
                QCOMPARE(evaluate("sounds[0]").toString(),
                         QString("You hear someone walking up to you."));
            }

            {
                player->go(roomB);

                QCOMPARE(evaluate("sounds.length").toInt32(), 1);
                QCOMPARE(evaluate("visuals.length").toInt32(), 5);
                QCOMPARE(evaluate("visuals[4]").toString(),
                         QString("Arie walks to the c-to-b."));
            }

            {
                player->go(roomA);

                QCOMPARE(evaluate("sounds.length").toInt32(), 1);
                QCOMPARE(evaluate("visuals.length").toInt32(), 5);
            }
        }
};

#endif // TEST_MOVEMENT_H
