#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>


class QWsServer;
class QWsSocket;

class WebSocketServer : public QObject {

    Q_OBJECT

    public:
        explicit WebSocketServer(quint16 port, QObject *parent = 0);
        virtual ~WebSocketServer();

    public slots:
        void onClientConnected();
        void onClientDisconnected();

        void onSessionOutput(const QString &data);

    private:
        QWsServer *m_server;
        QList<QWsSocket *> m_clients;
};

#endif // WEBSOCKETSERVER_H
