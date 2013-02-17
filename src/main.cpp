#include <csignal>

#include "application.h"
#include "engine.h"
#include "logutil.h"


void signalHandler(int param) {

    Q_UNUSED(param)

    LogUtil::logInfo("\nTerminating...");

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
