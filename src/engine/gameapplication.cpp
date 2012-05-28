#include "gameapplication.h"

#include <QDebug>

#include "gameexception.h"


GameApplication::GameApplication(int &argc, char **argv) :
    QCoreApplication(argc, argv) {
}

GameApplication::~GameApplication() {
}

bool GameApplication::notify(QObject *receiver, QEvent *event) {

    try {
        return receiver->event(event);
    } catch (const GameException &exception) {
        qWarning() << "Game Exception in event (" << event->type() << ") handler: "
                   << exception.what();
        return false;
    }
}
