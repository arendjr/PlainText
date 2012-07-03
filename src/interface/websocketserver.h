#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>


class QWsServer;
class QWsSocket;

class Realm;

class WebSocketServer : public QObject {

    Q_OBJECT

    public:
        WebSocketServer(Realm *realm, quint16 port, QObject *parent = nullptr);
        virtual ~WebSocketServer();

    public slots:
        void onClientConnected();
        void onClientDisconnected();

        void onSessionOutput(const QString &data);

    private:
        Realm *m_realm;

        QWsServer *m_server;
        QList<QWsSocket *> m_clients;
};

#endif // WEBSOCKETSERVER_H
