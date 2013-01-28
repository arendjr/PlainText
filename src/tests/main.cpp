#include <QTest>

#include "application.h"

#include "test_container.h"
#include "test_crashes.h"
#include "test_floodevent.h"
#include "test_help.h"
#include "test_movement.h"
#include "test_openandclose.h"
#include "test_serialization.h"
#include "test_visualevents.h"


int main(int argc, char *argv[]) {

    Application application(argc, argv);

    SerializationTest test1;
    ContainerTest test2;
    CrashesTest test3;
    VisualEventsTest test4;
    MovementTest test5;
    HelpTest test6;
    OpenAndCloseTest test7;
    FloodEventTest test8;

    QTest::qExec(&test1);
    QTest::qExec(&test2);
    QTest::qExec(&test3);
    QTest::qExec(&test4);
    QTest::qExec(&test5);
    QTest::qExec(&test6);
    QTest::qExec(&test7);
    QTest::qExec(&test8);

    return 0;
}
