#include "requesthandler.h"

#include <QTcpSocket>


RequestHandler::RequestHandler(QTcpSocket *socket) :
    QObject(),
    m_socket(socket) {

    connect(socket, SIGNAL(readyRead()), SLOT(processData()));
    connect(socket, SIGNAL(aboutToClose()), SLOT(deleteLater()));
}

void RequestHandler::processData() {

    QByteArray data = m_socket->readAll();
    qDebug("Read data: %s", data.constData());
}
