#include "engine.h"

#include <QDateTime>
#include <QMetaType>

#include "characterstats.h"
#include "gameobject.h"
#include "gameobjectptr.h"
#include "realm.h"
#include "scriptengine.h"
#include "util.h"
#include "interface/httpserver.h"
#include "interface/telnetserver.h"
#include "interface/websocketserver.h"


Engine::Engine() :
    QObject() {

    qsrand(QDateTime::currentMSecsSinceEpoch());

    qRegisterMetaType<CharacterStats>();
    qRegisterMetaType<GameObject *>();
    qRegisterMetaType<GameObjectPtr>();
    qRegisterMetaType<GameObjectPtrList>();

    m_scriptEngine = new ScriptEngine();
    m_realm = new Realm();
    m_util = new Util();

    m_realm->setScriptEngine(m_scriptEngine);
    m_realm->init();

    m_scriptEngine->setGlobalObject("Realm", m_realm);
    m_scriptEngine->setGlobalObject("Util", m_util);
}

void Engine::start() {

    m_telnetServer = new TelnetServer(m_realm, 4801);
    m_webSocketServer = new WebSocketServer(m_realm, 4802);
    m_httpServer = new HttpServer(8080);
}

Engine::~Engine() {

    delete m_httpServer;
    delete m_webSocketServer;
    delete m_telnetServer;

    m_scriptEngine->unsetGlobalObject("Realm");
    m_scriptEngine->unsetGlobalObject("Util");

    delete m_util;
    delete m_realm;
    delete m_scriptEngine;
}
