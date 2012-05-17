#include "engine.h"

#include <QMetaType>

#include "gameobject.h"
#include "gameobjectptr.h"
#include "realm.h"
#include "scriptengine.h"
#include "interface/httpserver.h"
#include "interface/websocketserver.h"


Engine::Engine(QObject *parent) :
    QObject(parent) {

    qRegisterMetaType<GameObject *>();
    qRegisterMetaType<GameObjectPtr>();
    qRegisterMetaType<GameObjectPtrList>();

    Realm::instantiate();

    ScriptEngine::instantiate();
}

void Engine::start() {

    m_webSocketServer = new WebSocketServer(4801);
    m_httpServer = new HttpServer(8080);
}

Engine::~Engine() {

    delete m_httpServer;
    delete m_webSocketServer;

    ScriptEngine::destroy();

    Realm::destroy();
}
