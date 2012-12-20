#include "testcase.h"

#include <QDir>
#include <QTest>

#include "diskutil.h"
#include "engine.h"
#include "player.h"
#include "portal.h"
#include "realm.h"
#include "room.h"
#include "scriptengine.h"


QScriptValue TestCase::evaluate(const QString &statement) {

    return ScriptEngine::instance()->evaluate(statement);
}

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
    bool started = m_engine->start(DontServe);

    QVERIFY2(started, "Engine should be started");

    createTestWorld();
}

void TestCase::cleanupTestCase() {

    destroyTestWorld();
    GameObject::clearPrototypeMap();

    delete m_engine;
}

void TestCase::createTestWorld() {

    Realm *realm = Realm::instance();

    Room *roomA = new Room(realm);
    roomA->setName("Room A");
    roomA->setPosition(Point3D(0, 0, 0));

    Room *roomB = new Room(realm);
    roomB->setName("Room B");
    roomB->setPosition(Point3D(0, 50, 0));

    Portal *portal = new Portal(realm);
    portal->setRoom(roomA);
    portal->setRoom2(roomB);
    portal->setName("a-to-b");
    portal->setName2("b-to-a");
    portal->setFlags(PortalFlags::CanPassThrough | PortalFlags::CanSeeThrough |
                     PortalFlags::CanHearThrough);

    roomA->addPortal(portal);
    roomB->addPortal(portal);

    Player *player = new Player(realm);
    player->setName("Arie");
    player->setCurrentRoom(roomA);
    player->setAdmin(true);

    Room *roomC = new Room(realm);
    roomC->setName("Room C");
    roomC->setPosition(Point3D(0, 100, 0));

    portal = new Portal(realm);
    portal->setRoom(roomB);
    portal->setRoom2(roomC);
    portal->setName("b-to-c");
    portal->setName2("c-to-b");
    portal->setFlags(PortalFlags::CanPassThrough | PortalFlags::CanSeeThrough |
                     PortalFlags::CanHearThrough);

    roomB->addPortal(portal);
    roomC->addPortal(portal);

    QCOMPARE(roomA->id(), (unsigned) 1);
    QCOMPARE(roomB->id(), (unsigned) 2);
    QCOMPARE(player->id(), (unsigned) 4);
    QCOMPARE(roomC->id(), (unsigned) 5);
    QCOMPARE(portal->id(), (unsigned) 6);
}

void TestCase::destroyTestWorld() {

    for (const GameObjectPtr &object : Realm::instance()->allObjects(GameObjectType::Unknown)) {
        if (object.isNull()) {
            continue;
        }
        object->setDeleted();
    }
}
