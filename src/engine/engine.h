#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>

#include "constants.h"


class HttpServer;
class LogUtil;
class Realm;
class ScriptEngine;
class TelnetServer;
class Util;
class WebSocketServer;

class Engine : public QObject {

    Q_OBJECT

    public:
        Engine();
        virtual ~Engine();

        bool start(Options options = NoOptions);

    private:
        HttpServer *m_httpServer;
        TelnetServer *m_telnetServer;
        WebSocketServer *m_webSocketServer;

        ScriptEngine *m_scriptEngine;
        Realm *m_realm;
        Util *m_util;
        LogUtil *m_logUtil;
};

#endif // ENGINE_H
