#ifndef TELNETSERVER_H
#define TELNETSERVER_H

#include <QObject>


class QTcpServer;
class QTcpSocket;

class TelnetServer : public QObject {

    Q_OBJECT

    public:
        explicit TelnetServer(quint16 port, QObject *parent = 0);
        virtual ~TelnetServer();

    public slots:
        void onClientConnected();
        void onReadyRead();
        void onClientDisconnected();

        void onSessionOutput(QString data);

    private:
        QTcpServer *m_server;
        QList<QTcpSocket *> m_clients;
};

#endif // TELNETSERVER_H
