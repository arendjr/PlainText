#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>


class HttpServer : public QTcpServer {

    Q_OBJECT

    public:
        explicit HttpServer(quint16 port = 80, QObject *parent = 0);
        virtual ~HttpServer();

    protected:
        virtual void incomingConnection(int socketDescriptor);

    private slots:
        void onReadyRead();
        void onDisconnected();
};

#endif // HTTPSERVER_H
