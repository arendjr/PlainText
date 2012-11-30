#ifndef TEST_CRASHES_H
#define TEST_CRASHES_H

#include "testcase.h"

#include <QDebug>
#include <QTest>

#include "gameobjectptr.h"
#include "realm.h"
#include "scriptengine.h"
#include "scriptfunction.h"


class CrashesTest : public TestCase {

    Q_OBJECT

    private slots:
        void testScriptCrashPotentials() {

            ScriptEngine::instance()->evaluate("$('player:4').go($('exit:123456789'));");

            ScriptEngine::instance()->evaluate("$('player:4').go($('exit:3'));");

            ScriptEngine::instance()->evaluate("$('player:4').go({ 'id': 123456789 });");

            ScriptEngine::instance()->evaluate("$('player:4').go({ 'id': 3 });");

            ScriptEngine::instance()->evaluate("$('player:4').go({ 'id': 0 });");

            ScriptEngine::instance()->evaluate("$('player:4').go({});");

            ScriptEngine::instance()->evaluate("$('player:4').go();");

            ScriptEngine::instance()->evaluate("$('player:4').go($('portal:123456789'));");

            ScriptEngine::instance()->evaluate("$('player:4').go($('portal:3'));");

            ScriptEngine::instance()->evaluate("$('room:123456789').go($('portal:3'));");

            ScriptEngine::instance()->evaluate("$('room:4').go($('portal:3'));");

            QVERIFY2(true, "Got here without crashing.");
        }

        void testPointerCrashPotentials() {

            Realm *realm = Realm::instance();

            int numExceptions = 0;

            try {
                GameObjectPtrList list;
                list.append(realm->getObject(GameObjectType::Room, 1));
                list.append(realm->getObject(GameObjectType::Portal, 3));
                list[2];
            } catch (GameException &exception) {
                qDebug() << exception.what();
                QCOMPARE(exception.cause(), GameException::IndexOutOfBounds);
                numExceptions++;
            }

            QCOMPARE(numExceptions, 1);
            QVERIFY2(true, "Got here without crashing.");
        }
};

#endif // TEST_CRASHES_H
