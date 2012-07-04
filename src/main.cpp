#include "engine/application.h"
#include "engine/engine.h"


int main(int argc, char *argv[]) {

    Application application(argc, argv);

    Engine engine;
    bool started = engine.start();
    if (!started) {
        return 1;
    }

    return application.exec();
}
