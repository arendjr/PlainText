#include "engine/engine.h"
#include "engine/gameapplication.h"


int main(int argc, char *argv[]) {

    GameApplication application(argc, argv);

    Engine engine;
    engine.start();

    return application.exec();
}
