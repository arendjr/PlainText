#ifndef TEST_VISUALEVENTS_H
#define TEST_VISUALEVENTS_H

#include "testcase.h"

#include <cmath>

#include <QDateTime>
#include <QDebug>
#include <QTest>

#include "character.h"
#include "portal.h"
#include "realm.h"
#include "room.h"
#include "util.h"
#include "visualevent.h"


class VisualEventsTest : public TestCase {

    Q_OBJECT

    private:
        void connectRooms(Room *roomA, Room *roomB) {

            Portal *portal = new Portal(Realm::instance());
            portal->setRoom(roomA);
            portal->setRoom2(roomB);
            portal->setName(Util::directionForVector(roomB->position() - roomA->position()));
            portal->setName2(Util::directionForVector(roomA->position() - roomB->position()));
            portal->setFlags(PortalFlags::CanSeeThrough);

            roomA->addPortal(portal);
            roomB->addPortal(portal);
        }

    private slots:
        virtual void init() {

            Realm *realm = Realm::instance();

            qint64 start = QDateTime::currentMSecsSinceEpoch();

            const int numRows = 100;
            const int numColumns = 100;
            for (int i = 0; i < numRows; i++) {
                int y = 20 * i;
                for (int j = 0; j < numColumns; j++) {
                    int x = 20 * j;
                    int z = 0 - 40 * sin(i * TAU / 200) - 40 * sin(j * TAU / 200);

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

            qint64 end = QDateTime::currentMSecsSinceEpoch();
            qDebug() << "Generating rooms took " << (end - start) << "ms";

            {
                Character *character = new Character(realm);
                character->setName("Character A");
                Room *room = m_rooms[0].cast<Room *>();
                room->addCharacter(character);
                character->setCurrentRoom(room);
                m_characters.append(character);
            }

            {
                Character *character = new Character(realm);
                character->setName("Character B");
                Room *room = m_rooms[numColumns - 1].cast<Room *>();
                room->addCharacter(character);
                character->setCurrentRoom(room);
                m_characters.append(character);
            }

            {
                Character *character = new Character(realm);
                character->setName("Character C");
                Room *room = m_rooms[(numRows - 1) * numColumns].cast<Room *>();
                room->addCharacter(character);
                character->setCurrentRoom(room);
                m_characters.append(character);
            }

            {
                Character *character = new Character(realm);
                character->setName("Character D");
                Room *room = m_rooms[numRows * numColumns - 1].cast<Room *>();
                room->addCharacter(character);
                character->setCurrentRoom(room);
                m_characters.append(character);
            }
        }

        virtual void cleanup() {

            m_rooms.clear();
            m_characters.clear();
        }

        void testVisualEvent() {

            Realm *realm = Realm::instance();
            realm->enqueueModifiedObjects();

            int numRooms = m_rooms.length();
            QCOMPARE(numRooms, 10000);

            Room *room = m_rooms[numRooms / 2].cast<Room *>();
            VisualEvent *event = new VisualEvent(room, 100.0);
            event->setDescription("You see a bright white flash.");

            qint64 start = QDateTime::currentMSecsSinceEpoch();

            event->fire();

            qint64 end = QDateTime::currentMSecsSinceEpoch();
            qDebug() << "Event fire took " << (end - start) << "ms";

            QCOMPARE(event->numVisitedRooms(), 10000);
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
