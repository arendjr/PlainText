#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QSslError>
#include <QTcpServer>


class HttpServer : public QTcpServer {

    Q_OBJECT

    public:
        explicit HttpServer(QObject *parent = 0);

        void start();

    protected:
        virtual void incomingConnection(int socketDescriptor);

    private slots:
        void sslErrors(QList<QSslError> errors);
        void socketError(QAbstractSocket::SocketError error);
        void socketEncrypted();

        void handleNewConnection();
};

#endif // HTTPSERVER_H
