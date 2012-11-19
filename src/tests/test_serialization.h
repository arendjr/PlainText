#include "testcase.h"

#include <QTest>

#include "realm.h"


class SerializationTest: public TestCase {

    Q_OBJECT

    private slots:
        void realmExists() {

            QVERIFY(Realm::instance() != nullptr);
        }
};
