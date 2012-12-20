#ifndef TEST_OPENANDCLOSE_H
#define TEST_OPENANDCLOSE_H

#include "testcase.h"

#include <QDebug>
#include <QSignalSpy>
#include <QTest>

#include "player.h"
#include "portal.h"
#include "realm.h"
#include "room.h"
#include "session.h"


class OpenAndCloseTest : public TestCase {

    Q_OBJECT

    private slots:
        void testOpenAndClose() {

            Realm *realm = Realm::instance();
            Session *session = new Session(realm, "Mock", "", this);
            Player *player = (Player *) realm->getPlayer("Arie");
            player->setSession(session);

            Portal *portal = (Portal *) realm->getObject(GameObjectType::Portal, 3);
            portal->setFlags(PortalFlags::CanOpenFromSide1 |
                             PortalFlags::CanOpenFromSide2 |
                             PortalFlags::CanSeeThroughIfOpen |
                             PortalFlags::CanHearThroughIfOpen |
                             PortalFlags::CanShootThroughIfOpen |
                             PortalFlags::CanPassThroughIfOpen);

            QSignalSpy spy(player->session(), SIGNAL(write(QString)));

            QCOMPARE(player->currentRoom()->name(), QString("Room A"));

            {
                player->execute("go a-to-b");

                QCOMPARE(spy.count(), 1);
                QList<QVariant> arguments = spy.takeFirst();
                QCOMPARE(arguments[0].toString().trimmed(), QString("The a-to-b is closed."));

                QCOMPARE(player->currentRoom()->name(), QString("Room A"));
            }

            {
                player->execute("open a-to-b");

                QCOMPARE(spy.count(), 1);
                QList<QVariant> arguments = spy.takeFirst();
                QCOMPARE(arguments[0].toString().trimmed(), QString("You open the a-to-b."));
            }

            {
                player->execute("open a-to-b");

                QCOMPARE(spy.count(), 1);
                QList<QVariant> arguments = spy.takeFirst();
                QCOMPARE(arguments[0].toString().trimmed(), QString("The a-to-b is already open."));
            }

            {
                player->execute("go a-to-b");

                spy.takeFirst();

                QCOMPARE(player->currentRoom()->name(), QString("Room B"));
            }

            {
                player->execute("close b-to-a");

                QList<QVariant> arguments = spy.takeFirst();
                QCOMPARE(arguments[0].toString().trimmed(), QString("You close the b-to-a."));
            }

            {
                player->execute("close b-to-a");

                QList<QVariant> arguments = spy.takeFirst();
                QCOMPARE(arguments[0].toString().trimmed(),
                         QString("The b-to-a is already closed."));
            }

            {
                player->execute("go b-to-a");

                QList<QVariant> arguments = spy.takeFirst();
                QCOMPARE(arguments[0].toString().trimmed(), QString("The b-to-a is closed."));

                QCOMPARE(player->currentRoom()->name(), QString("Room B"));
            }
        }
};

#endif // TEST_OPENANDCLOSE_H
