#ifndef TEST_CONTAINER_H
#define TEST_CONTAINER_H

#include "testcase.h"

#include <QSignalSpy>
#include <QTest>

#include "container.h"
#include "player.h"
#include "realm.h"


class ContainerTest : public TestCase {

    Q_OBJECT

    private slots:
        virtual void init() {

            Realm *realm = Realm::instance();
            Player *player = realm->getPlayer("Arie");

            m_container = new Container(realm);
            m_container->setName("container");
            player->addInventoryItem(m_container);

            Item *item1 = new Item(realm);
            item1->setName("item1");
            m_items.append(item1);
            player->addInventoryItem(item1);

            Item *item2 = new Item(realm);
            item2->setName("item2");
            m_items.append(item2);
            player->addInventoryItem(item2);

            Item *item3 = new Item(realm);
            item3->setName("item3");
            m_items.append(item3);
            player->addInventoryItem(item3);
        }

        void testPutAllInContainer() {

            Realm *realm = Realm::instance();
            Player *player = realm->getPlayer("Arie");

            QSignalSpy spy(player, SIGNAL(write(QString)));

            player->execute("put all in container");

            QCOMPARE(spy.count(), 1);
            QList<QVariant> arguments = spy.takeFirst();
            QCOMPARE(arguments[0].toString().trimmed(),
                     QString("You put the item1, the item2 and the item3 in the container."));

            QVERIFY(player->inventory().contains(m_container));
            QVERIFY(!player->inventory().contains(m_items[0]));
            QVERIFY(!player->inventory().contains(m_items[1]));
            QVERIFY(!player->inventory().contains(m_items[2]));

            QVERIFY(!m_container->items().contains(m_container));
            QVERIFY(m_container->items().contains(m_items[0]));
            QVERIFY(m_container->items().contains(m_items[1]));
            QVERIFY(m_container->items().contains(m_items[2]));
        }

    private:
        Container *m_container;

        GameObjectPtrList m_items;
};

#endif // TEST_CONTAINER_H
