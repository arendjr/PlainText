#include "application.h"

#include "gameexception.h"
#include "logutil.h"


Application::Application(int &argc, char **argv) :
    QCoreApplication(argc, argv) {
}

Application::~Application() {
}

bool Application::notify(QObject *receiver, QEvent *event) {

    try {
        return receiver->event(event);
    } catch (GameException &exception) {
        LogUtil::logError("Game Exception in event handler: %1", exception.what());
    } catch (...) {
        LogUtil::logError("Unknown Exception in event handler.");
    }
    return false;
}
