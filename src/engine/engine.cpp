#include "engine.h"

#include <QDateTime>
#include <QMetaType>

#include "gameobject.h"
#include "gameobjectptr.h"
#include "realm.h"
#include "scriptengine.h"
#include "interface/httpserver.h"
#include "interface/telnetserver.h"
#include "interface/websocketserver.h"


Engine::Engine(QObject *parent) :
    QObject(parent) {

    qsrand(QDateTime::currentMSecsSinceEpoch());

    qRegisterMetaType<GameObject *>();
    qRegisterMetaType<GameObjectPtr>();
    qRegisterMetaType<GameObjectPtrList>();

    ScriptEngine::instantiate();

    Realm::instantiate();
}

void Engine::start() {

    m_telnetServer = new TelnetServer(4801);
    m_webSocketServer = new WebSocketServer(4802);
    m_httpServer = new HttpServer(8080);
}

Engine::~Engine() {

    delete m_httpServer;
    delete m_webSocketServer;
    delete m_telnetServer;

    Realm::destroy();

    ScriptEngine::destroy();
}
