#include <QCoreApplication>

#include "engine/engine.h"


int main(int argc, char *argv[]) {

    QCoreApplication application(argc, argv);

    Engine engine;
    engine.start();

    return application.exec();
}
