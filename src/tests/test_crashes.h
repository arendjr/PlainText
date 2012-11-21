#ifndef TEST_CRASHES_H
#define TEST_CRASHES_H

#include "testcase.h"

#include <QTest>

#include "scriptengine.h"
#include "scriptfunction.h"


class CrashesTest : public TestCase {

    Q_OBJECT

    private slots:
        void testCrashPotentials() {

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
};

#endif // TEST_CRASHES_H
