#include "httpserver.h"
#include "requesthandler.h"

#include <QSslCipher>
#include <QSslSocket>


HttpServer::HttpServer(QObject *parent) :
    QTcpServer(parent) {

    connect(this, SIGNAL(newConnection()), SLOT(handleNewConnection()));
}

void HttpServer::start() {

    bool result = listen();
    if (!result) {
        qCritical("Couldn't start HTTP server.");
        return;
    }

    QSslKey key;
    QSslSocket::setPrivateKey(privateKey);

    qDebug("HTTP server listening on port %d.", serverPort());

    qDebug("Enabled ciphers:");
    foreach (QSslCipher cipher, QSslSocket::defaultCiphers()) {
        qDebug("  %s (%s, %s, %s)", cipher.name().toLatin1().constData(),
               cipher.protocolString().toLatin1().constData(),
               cipher.authenticationMethod().toLatin1().constData(),
               cipher.encryptionMethod().toLatin1().constData());
    }
}

void HttpServer::incomingConnection(int socketDescriptor) {

    qDebug("New socket opened.");
    QSslSocket *socket = new QSslSocket(this);
    if (socket->setSocketDescriptor(socketDescriptor)) {
        //socket->setProtocol(QSsl::TlsV1);
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
                SLOT(socketError(QAbstractSocket::SocketError)));
        connect(socket, SIGNAL(sslErrors(QList<QSslError>)),
                SLOT(sslErrors(QList<QSslError>)));
        connect(socket, SIGNAL(encrypted()), SLOT(socketEncrypted()));
        socket->startServerEncryption();
    } else {
        qWarning("Failed to set socket descriptor. Closing socket...");
        delete socket;
    }
}

void HttpServer::sslErrors(QList<QSslError> errors) {

    QSslSocket *socket = qobject_cast<QSslSocket *>(sender());
    Q_ASSERT(socket);

    foreach (QSslError error, errors) {
        qDebug("SSL error: %s (%d)", error.errorString().toLatin1().constData(), error.error());
    }

    socket->deleteLater();
}

void HttpServer::socketError(QAbstractSocket::SocketError error) {

    QSslSocket *socket = qobject_cast<QSslSocket *>(sender());
    Q_ASSERT(socket);

    qDebug("Socket error: %s (%d)", socket->errorString().toLatin1().constData(), error);
    socket->deleteLater();
}

void HttpServer::socketEncrypted() {

    qDebug("New socket encrypted.");
    QSslSocket *socket = qobject_cast<QSslSocket *>(sender());
    Q_ASSERT(socket);

    addPendingConnection(socket);
    emit newConnection();
}

void HttpServer::handleNewConnection() {

    QTcpSocket *socket = nextPendingConnection();
    if (socket) {
        qDebug("New socket encrypted and ready to go.");
        new RequestHandler(socket);
    } else {
        qDebug("handleNewConnection() called, but no pending connection...");
    }
}
