#ifndef TEST_HELP_H
#define TEST_HELP_H

#include "testcase.h"

#include <QSignalSpy>
#include <QTest>

#include "player.h"
#include "portal.h"
#include "realm.h"
#include "room.h"
#include "session.h"
#include "util.h"


class HelpTest : public TestCase {

    Q_OBJECT

    private slots:
        void testHelp() {

            Realm *realm = Realm::instance();
            Session *session = new Session(realm, "Mock", "", this);
            Player *player = (Player *) realm->getPlayer("Arie");
            player->setSession(session);

            QSignalSpy spy(player->session(), SIGNAL(write(QString)));

            player->execute("help open");

            QCOMPARE(spy.count(), 1);
            QList<QVariant> arguments = spy.takeFirst();
            QCOMPARE(arguments[0].toString().trimmed(),
                     QString(Util::highlight("open") + "\n"
                             "  Open an exit, typically a door or a window. Note that doors may\n"
                             "  automatically close after a while.\n"
                             "  \n"
                             "  Example: open door"));
        }
};

#endif // TEST_HELP_H
