#include "testcase.h"

#include <QDir>
#include <QTest>

#include "diskutil.h"
#include "engine.h"
#include "player.h"
#include "portal.h"
#include "realm.h"
#include "room.h"


void TestCase::initTestCase() {

    if (qgetenv("PT_DATA_DIR").isEmpty()) {
        qputenv("PT_DATA_DIR", "data");
    }

    for (const QString &fileName : DiskUtil::dataDirFileList()) {
        if (!fileName.startsWith("realm.")) {
            QFile::remove(DiskUtil::dataDir() + "/" + fileName);
        }
    }

    m_engine = new Engine();
    bool started = m_engine->start();

    QVERIFY2(started, "Engine should be started");

    createTestWorld();
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
    roomB->setName("Room B");

    Portal *portal = new Portal(realm);
    portal->setRoom(roomA);
    portal->setRoom2(roomB);
    portal->setName("a-to-b");
    portal->setName2("b-to-a");
    portal->setFlags(PortalFlags::CanPassThrough);

    roomA->addPortal(portal);
    roomB->addPortal(portal);

    Player *player = new Player(realm, 1999);
    player->setName("Arie");
    player->setCurrentRoom(roomA);
    player->setPasswordSalt("W6YS83vT");
    player->setPasswordHash("CNPnlGobv2Q8VUH2kOovwPvZqk0=");
    player->setAdmin(true);

    QCOMPARE(roomA->id(), (unsigned) 1);
    QCOMPARE(roomB->id(), (unsigned) 2);
    QCOMPARE(portal->id(), (unsigned) 3);
    QCOMPARE(player->id(), (unsigned) 1999);
}

void TestCase::destroyTestWorld() {

    for (const GameObjectPtr &object : Realm::instance()->allObjects(GameObjectType::Unknown)) {
        if (object.isNull()) {
            continue;
        }
        object->setDeleted();
    }
}
