#include <QTest>

#include "application.h"

#include "test_container.h"
#include "test_serialization.h"


int main(int argc, char *argv[]) {

    Application application(argc, argv);

    SerializationTest test1;
    ContainerTest test2;

    QTest::qExec(&test1);
    QTest::qExec(&test2);

    return 0;
}
