#ifndef ENGINE_H
#define ENGINE_H

#include <QObject>


class HttpServer;
class TelnetServer;
class WebSocketServer;

class Engine : public QObject {

    Q_OBJECT

    public:
        explicit Engine(QObject *parent = 0);
        virtual ~Engine();

        void start();

    private:
        HttpServer *m_httpServer;
        TelnetServer *m_telnetServer;
        WebSocketServer *m_webSocketServer;
};

#endif // ENGINE_H
