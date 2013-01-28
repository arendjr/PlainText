#ifndef TEST_FLOODEVENT_H
#define TEST_FLOODEVENT_H

#include "testcase.h"

#include <cmath>

#include <QDateTime>
#include <QDebug>
#include <QTest>

#include "character.h"
#include "floodevent.h"
#include "portal.h"
#include "realm.h"
#include "room.h"
#include "util.h"


class FloodEventTest : public TestCase {

    Q_OBJECT

    private:
        void connectRooms(Room *roomA, Room *roomB) {

            Portal *portal = new Portal(Realm::instance());
            portal->setRoom(roomA);
            portal->setRoom2(roomB);
            portal->setName(Util::directionForVector(roomB->position() - roomA->position()));
            portal->setName2(Util::directionForVector(roomA->position() - roomB->position()));
            portal->setFlags(PortalFlags::CanPassThrough);

            roomA->addPortal(portal);
            roomB->addPortal(portal);
        }

    private slots:
        virtual void init() {

            Realm *realm = Realm::instance();

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
        }

        virtual void cleanup() {

            m_rooms.clear();
            m_characters.clear();
        }

        void testFloodEvent() {

            Realm *realm = Realm::instance();
            realm->enqueueModifiedObjects();

            int numRooms = m_rooms.length();
            QCOMPARE(numRooms, 10000);

            Room *room = m_rooms[numRooms / 2].cast<Room *>();
            FloodEvent *event = new FloodEvent(room, 0.1);
            event->setDescription("The water is up above your waist");
            event->setDistantDescription("There's a considerable amount of water here");
            event->setVeryDistantDescription("There's a little water on the ground");

            {
                qint64 start = QDateTime::currentMSecsSinceEpoch();

                event->fire();

                qint64 end = QDateTime::currentMSecsSinceEpoch();
                qDebug() << "Event fire took " << (end - start) << "ms";
            }

            {
                qint64 start = QDateTime::currentMSecsSinceEpoch();

                realm->enqueueModifiedObjects();

                qint64 end = QDateTime::currentMSecsSinceEpoch();
                qDebug() << "Queuing modified objects took " << (end - start) << "ms";
            }

            QCOMPARE(event->numVisitedRooms(), 10000);
        }

    private:
        GameObjectPtrList m_rooms;
        GameObjectPtrList m_characters;
};

#endif // TEST_FLOODEVENT_H
