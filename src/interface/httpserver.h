#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QTcpServer>


class HttpServer : public QTcpServer {

    Q_OBJECT

    public:
        HttpServer(quint16 port, quint16 webSocketPort, QObject *parent = nullptr);
        virtual ~HttpServer();

    protected:
        virtual void incomingConnection(int socketDescriptor);

    private slots:
        void onReadyRead();
        void onDisconnected();

    private:
        QByteArray m_title;
        quint16 m_webSocketPort;
};

#endif // HTTPSERVER_H
