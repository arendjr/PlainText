#include "engine.h"

#include <QDateTime>
#include <QDebug>
#include <QMetaType>

#include "commandregistry.h"
#include "gameexception.h"
#include "logutil.h"
#include "realm.h"
#include "scriptengine.h"
#include "triggerregistry.h"
#include "util.h"
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
    m_util(nullptr),
    m_logUtil(nullptr) {

    qsrand(QDateTime::currentMSecsSinceEpoch());
}

bool Engine::start(Options options) {

    try {
        m_scriptEngine = new ScriptEngine();
        m_realm = new Realm();
        m_util = new Util();
        m_logUtil = new LogUtil();

        m_realm->setScriptEngine(m_scriptEngine);
        m_scriptEngine->setGlobalObject("CommandRegistry", m_realm->commandRegistry());
        m_scriptEngine->setGlobalObject("LogUtil", m_logUtil);
        m_scriptEngine->setGlobalObject("Realm", m_realm);
        m_scriptEngine->setGlobalObject("TriggerRegistry", m_realm->triggerRegistry());
        m_scriptEngine->setGlobalObject("Util", m_util);

        m_scriptEngine->loadScripts();
        m_realm->init();

        if (~options & DontServe) {
            m_telnetServer = new TelnetServer(m_realm, 4801);
            m_webSocketServer = new WebSocketServer(m_realm, 4802);
            m_httpServer = new HttpServer(8080);
        }

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

    m_scriptEngine->unsetGlobalObject("CommandRegistry");
    m_scriptEngine->unsetGlobalObject("LogUtil");
    m_scriptEngine->unsetGlobalObject("Realm");
    m_scriptEngine->unsetGlobalObject("TriggerRegistry");
    m_scriptEngine->unsetGlobalObject("Util");

    delete m_logUtil;
    delete m_util;
    delete m_realm;
    delete m_scriptEngine;
}
