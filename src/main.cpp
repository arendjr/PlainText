#include <csignal>

#include <QDebug>

#include "engine/application.h"
#include "engine/engine.h"


void signalHandler(int param) {

    Q_UNUSED(param)

    qDebug() << endl << "Terminating...";

    qApp->quit();
}

int main(int argc, char *argv[]) {

    Application application(argc, argv);

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    Engine engine;
    bool started = engine.start();
    if (!started) {
        return 1;
    }

    return application.exec();
}
