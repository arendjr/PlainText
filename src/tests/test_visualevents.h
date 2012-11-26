#ifndef TEST_VISUALEVENTS_H
#define TEST_VISUALEVENTS_H

#include "testcase.h"

#include <cmath>

#include <QTest>

#include "character.h"
#include "portal.h"
#include "realm.h"
#include "room.h"
#include "util.h"
#include "visualevent.h"


#define TAU 6.2831853071


class VisualEventsTest : public TestCase {

    Q_OBJECT

    private:
        void connectRooms(Room *roomA, Room *roomB) {

            Portal *portal = new Portal(Realm::instance());
            portal->setRoom(roomA);
            portal->setRoom2(roomB);
            portal->setName(Util::directionForVector(roomB->position() - roomA->position()));
            portal->setName2(Util::directionForVector(roomA->position() - roomB->position()));

            roomA->addPortal(portal);
            roomB->addPortal(portal);
        }

    private slots:
        virtual void init() {

            Realm *realm = Realm::instance();

            const int numRows = 120;
            const int numColumns = 120;
            for (int i = 0; i < numRows; i++) {
                int y = 20 * i;
                for (int j = 0; j < numColumns; j++) {
                    int x = 20 * j;
                    int z = 0 - 50 * sin(i * TAU / 240) - 50 * sin(j * TAU / 240);

                    Room *room = new Room(realm);
                    room->setPosition(Point3D(x, y, z));

                    if (i > 0) {
                        int previousRow = i - 1;
                        connectRooms(m_rooms[previousRow * numColumns + j].cast<Room *>(), room);
                        if (j > 0) {
                            connectRooms(m_rooms[previousRow * numColumns + j - 1].cast<Room *>(),
                                         room);
                        }
                        if (j < numColumns - 1) {
                            connectRooms(m_rooms[previousRow * numColumns + j + 1].cast<Room *>(),
                                         room);
                        }
                    }
                    if (j > 0) {
                        connectRooms(m_rooms[i * numColumns + j - 1].cast<Room *>(), room);
                    }
                    m_rooms.append(room);
                }
            }

            {
                Character *character = new Character(realm);
                character->setName("Character A");
                m_characters.append(character);
            }

            {
                Character *character = new Character(realm);
                character->setName("Character B");
                m_characters.append(character);
            }

            {
                Character *character = new Character(realm);
                character->setName("Character C");
                m_characters.append(character);
            }

            {
                Character *character = new Character(realm);
                character->setName("Character D");
                m_characters.append(character);
            }
        }

        void testVisualEvent() {

            int numRooms = m_rooms.length();
            QCOMPARE(numRooms, 14400);

            Room *room = m_rooms[numRooms / 2].cast<Room *>();
            VisualEvent *event = new VisualEvent(room, 100.0);
            event->setDescription("You see a bright white flash.");

            QBENCHMARK {
                event->fire();
            }

            QVERIFY(event->affectedCharacters().contains(m_characters[0]));
            QVERIFY(event->affectedCharacters().contains(m_characters[1]));
            QVERIFY(event->affectedCharacters().contains(m_characters[2]));
            QVERIFY(event->affectedCharacters().contains(m_characters[3]));
        }

    private:
        GameObjectPtrList m_rooms;
        GameObjectPtrList m_characters;
};

#endif // TEST_VISUALEVENTS_H
