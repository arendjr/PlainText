#include "engine.h"

#include <QDateTime>
#include <QMetaType>

#include "commandregistry.h"
#include "diskutil.h"
#include "gameexception.h"
#include "httpserver.h"
#include "logutil.h"
#include "realm.h"
#include "scriptengine.h"
#include "telnetserver.h"
#include "triggerregistry.h"
#include "util.h"
#include "websocketserver.h"


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
        LogUtil::setLoggingEnabled(!DiskUtil::logDir().isEmpty());

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
            quint16 telnetPort = qgetenv("PT_TELNET_PORT").toUInt();
            quint16 webSocketPort = qgetenv("PT_WEBSOCKET_PORT").toUInt();
            quint16 httpPort = qgetenv("PT_HTTP_PORT").toUInt();

            if (telnetPort == 0) {
                telnetPort = 4801;
            }
            if (webSocketPort == 0) {
                webSocketPort = 4802;
            }
            if (httpPort == 0) {
                httpPort = 8080;
            }

            m_telnetServer = new TelnetServer(m_realm, telnetPort);
            m_webSocketServer = new WebSocketServer(m_realm, webSocketPort);
            m_httpServer = new HttpServer(httpPort, webSocketPort);
        }

        return true;
    } catch (const GameException &exception) {
        LogUtil::logError("Could not start engine: %1", exception.what());
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
