#include "engine.h"

#include <QDateTime>
#include <QDebug>
#include <QMetaType>

#include "characterstats.h"
#include "commandregistry.h"
#include "gameexception.h"
#include "gameobject.h"
#include "gameobjectptr.h"
#include "point3d.h"
#include "realm.h"
#include "scriptengine.h"
#include "util.h"
#include "vector3d.h"
#include "interface/httpserver.h"
#include "interface/telnetserver.h"
#include "interface/websocketserver.h"


Engine::Engine() :
    QObject(),
    m_httpServer(nullptr),
    m_telnetServer(nullptr),
    m_webSocketServer(nullptr),
    m_scriptEngine(nullptr),
    m_realm(nullptr),
    m_util(nullptr) {

    qsrand(QDateTime::currentMSecsSinceEpoch());

    qRegisterMetaType<CharacterStats>();
    qRegisterMetaType<GameObject *>();
    qRegisterMetaType<GameObjectPtr>();
    qRegisterMetaType<GameObjectPtrList>();
    qRegisterMetaType<Point3D>();
    qRegisterMetaType<Vector3D>();
}

bool Engine::start() {

    try {
        m_scriptEngine = new ScriptEngine();
        m_realm = new Realm();
        m_util = new Util();

        m_realm->setScriptEngine(m_scriptEngine);
        m_scriptEngine->setGlobalObject("CommandRegistry", m_realm->commandRegistry());
        m_scriptEngine->setGlobalObject("Realm", m_realm);
        m_scriptEngine->setGlobalObject("Util", m_util);

        m_scriptEngine->loadScripts();
        m_realm->init();

        m_telnetServer = new TelnetServer(m_realm, 4801);
        m_webSocketServer = new WebSocketServer(m_realm, 4802);
        m_httpServer = new HttpServer(8080);

        return true;
    } catch (const GameException &exception) {
        qWarning() << "Could not start engine:" << exception.what();
        return false;
    }
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
