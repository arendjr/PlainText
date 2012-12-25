#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>


class HttpServer : public QTcpServer {

    Q_OBJECT

    public:
        HttpServer(quint16 port = 80, QObject *parent = nullptr);
        virtual ~HttpServer();

    protected:
        virtual void incomingConnection(int socketDescriptor);

    private slots:
        void onReadyRead();
        void onDisconnected();

    private:
        QByteArray m_title;
};

#endif // HTTPSERVER_H
