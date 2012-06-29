#include "application.h"

#include <QDebug>

#include "gameexception.h"


Application::Application(int &argc, char **argv) :
    QCoreApplication(argc, argv) {
}

Application::~Application() {
}

bool Application::notify(QObject *receiver, QEvent *event) {

    try {
        return receiver->event(event);
    } catch (GameException &exception) {
        qWarning() << "Game Exception in event handler: " + QString(exception.what());
    } catch (...) {
        qWarning() << "Unknown exception in event handler.";
    }
    return false;
}
