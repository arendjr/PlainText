#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>

class QTcpSocket;


class RequestHandler : public QObject {

    Q_OBJECT

    public:
        explicit RequestHandler(QTcpSocket *socket);

    public slots:
        void processData();

    private:
        QTcpSocket *m_socket;
};

#endif // REQUESTHANDLER_H
