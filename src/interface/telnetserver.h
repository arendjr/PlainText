#ifndef TELNETSERVER_H
#define TELNETSERVER_H

#include <QObject>


class QTcpServer;
class QTcpSocket;

class Realm;

class TelnetServer : public QObject {

    Q_OBJECT

    public:
        TelnetServer(Realm *realm, quint16 port, QObject *parent = nullptr);
        virtual ~TelnetServer();

    public slots:
        void onClientConnected();
        void onReadyRead();
        void onClientDisconnected();

        void onSessionOutput(QString data);

    private:
        Realm *m_realm;

        QTcpServer *m_server;
        QList<QTcpSocket *> m_clients;
};

#endif // TELNETSERVER_H
