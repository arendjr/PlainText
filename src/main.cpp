#include "engine/application.h"
#include "engine/engine.h"


int main(int argc, char *argv[]) {

    Application application(argc, argv);

    Engine engine;
    engine.start();

    return application.exec();
}
