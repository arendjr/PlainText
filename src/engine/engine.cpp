#include "engine.h"

#include <QMetaType>

#include "exit.h"
#include "gameobjectptr.h"
#include "realm.h"
#include "interface/httpserver.h"
#include "interface/websocketserver.h"


Engine::Engine(QObject *parent) :
    QObject(parent) {

    qRegisterMetaType<Exit>();
    qRegisterMetaType<ExitList>();
    qRegisterMetaType<GameObjectPtr>();
    qRegisterMetaType<GameObjectPtrList>();

    Realm::instance();
}

void Engine::start() {

    m_webSocketServer = new WebSocketServer(4801);
    m_httpServer = new HttpServer(8080);
}

Engine::~Engine() {

    delete m_httpServer;
    delete m_webSocketServer;

    Realm::destroy();
}
