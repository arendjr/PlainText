#include <QTest>

#include "application.h"

#include "test_serialization.h"


int main(int argc, char *argv[]) {

    Application application(argc, argv);

    SerializationTest test1;

    QTest::qExec(&test1);

    return 0;
}
