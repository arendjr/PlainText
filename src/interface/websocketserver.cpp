#include "websocketserver.h"

#include <QCoreApplication>
#include <QDebug>

#include <QWsServer.h>
#include <QWsSocket.h>

#include "session.h"
#include "engine/player.h"
#include "engine/util.h"


WebSocketServer::WebSocketServer(quint16 port, QObject *parent) :
    QObject(parent) {

    m_server = new QWsServer(this);
    if (!m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "Error: Can't launch WebSocket server";
    } else {
        qDebug() << "WebSocket server is listening on port " + QString::number(port);
    }

    connect(m_server, SIGNAL(newConnection()), this, SLOT(onClientConnected()));
}

WebSocketServer::~WebSocketServer() {
}

void WebSocketServer::onClientConnected() {

    QWsSocket *socket = m_server->nextPendingConnection();
    connect(socket, SIGNAL(disconnected()), SLOT(onClientDisconnected()));

    Session *session = new Session(socket);
    connect(session, SIGNAL(write(QString)), SLOT(onSessionOutput(QString)));

    connect(socket, SIGNAL(frameReceived(QString)), session, SLOT(onUserInput(QString)));
    connect(session, SIGNAL(terminate()), socket, SLOT(close()));

    session->open();

    m_clients << socket;
}

void WebSocketServer::onClientDisconnected() {

    QWsSocket *socket = qobject_cast<QWsSocket *>(sender());
    if (socket == 0) {
        return;
    }

    m_clients.removeOne(socket);

    socket->deleteLater();
}

void WebSocketServer::onSessionOutput(const QString &data) {

    Session *session = qobject_cast<Session *>(sender());
    if (session == 0) {
        return;
    }

    QWsSocket *socket = qobject_cast<QWsSocket *>(session->parent());
    if (socket == 0) {
        return;
    }

    socket->write(data);

    if (session->authenticated()) {
        Player *player = session->player();
        Q_ASSERT(player);

        socket->write(QString("{ \"player\": { \"name\": %1, \"isAdmin\": %2, \"hp\": %3 } }")
                      .arg(Util::jsString(player->name()),
                           player->isAdmin() ? "true" : "false",
                           QString::number(player->hp())));
    }
}
