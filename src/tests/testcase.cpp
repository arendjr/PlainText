#include "testcase.h"

#include <QTest>

#include "engine.h"
#include "player.h"
#include "portal.h"
#include "realm.h"
#include "room.h"


void TestCase::initTestCase() {

    qputenv("PT_DATA_DIR", "data");

    m_engine = new Engine();
    bool started = m_engine->start();

    QVERIFY2(started, "Engine should be started");

    createTestWorld();
}

void TestCase::init() {
}

void TestCase::cleanup() {
}

void TestCase::cleanupTestCase() {

    destroyTestWorld();

    delete m_engine;
}

void TestCase::createTestWorld() {

    Realm *realm = Realm::instance();

    Room *roomA = new Room(realm);
    roomA->setName("Room A");

    Room *roomB = new Room(realm);
    roomA->setName("Room B");

    Portal *portal = new Portal(realm);
    portal->setRoom(roomA);
    portal->setRoom2(roomB);
    portal->setName("a-to-b");
    portal->setName2("b-to-a");
    portal->setFlags(PortalFlags::CanPassThrough);

    roomA->addPortal(portal);
    roomB->addPortal(portal);

    Player *player = new Player(realm);
    player->setName("Arie");
    player->setCurrentRoom(roomA);
}

void TestCase::destroyTestWorld() {

    for (const GameObjectPtr &object : Realm::instance()->allObjects(GameObjectType::Unknown)) {
        if (object.isNull()) {
            continue;
        }
        object->setDeleted();
    }
}
