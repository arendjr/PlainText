#include "websocketserver.h"

#include <QCoreApplication>
#include <QDebug>

#include <QWsServer.h>
#include <QWsSocket.h>

#include "conversionutil.h"
#include "player.h"
#include "session.h"


WebSocketServer::WebSocketServer(Realm *realm, quint16 port, QObject *parent) :
    QObject(parent),
    m_realm(realm) {

    m_server = new QWsServer(this);
    if (!m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "Error: Can't launch WebSocket server";
    } else {
        qDebug() << "WebSocket server is listening on port" << port;
    }

    connect(m_server, SIGNAL(newConnection()), this, SLOT(onClientConnected()));
}

WebSocketServer::~WebSocketServer() {
}

void WebSocketServer::onClientConnected() {

    QWsSocket *socket = m_server->nextPendingConnection();
    connect(socket, SIGNAL(disconnected()), SLOT(onClientDisconnected()));

    Session *session = new Session(m_realm, "WebSocket", socket->peerAddress().toString(), socket);
    connect(session, SIGNAL(write(QString)), SLOT(onSessionOutput(QString)));

    connect(socket, SIGNAL(frameReceived(QString)), session, SLOT(onUserInput(QString)));
    connect(session, SIGNAL(terminate()), socket, SLOT(close()));

    session->open();

    m_clients << socket;
}

void WebSocketServer::onClientDisconnected() {

    QWsSocket *socket = qobject_cast<QWsSocket *>(sender());
    if (!socket) {
        return;
    }

    m_clients.removeOne(socket);

    socket->deleteLater();
}

void WebSocketServer::onSessionOutput(const QString &data) {

    Session *session = qobject_cast<Session *>(sender());
    if (!session) {
        return;
    }

    QWsSocket *socket = qobject_cast<QWsSocket *>(session->parent());
    if (!socket) {
        return;
    }

    if (!data.trimmed().isEmpty()) {
        socket->write(data);
    }

    if (session->authenticated() &&
        !(data.startsWith("{") && data.endsWith("}"))) {
        Player *player = session->player();
        Q_ASSERT(player);

        socket->write(QString("{ "
                                "\"player\": { "
                                  "\"name\": %1, "
                                  "\"isAdmin\": %2, "
                                  "\"hp\": %3, "
                                  "\"maxHp\": %4, "
                                  "\"mp\": %5, "
                                  "\"maxMp\": %6 "
                                "} "
                              "}")
                      .arg(ConversionUtil::jsString(player->name()),
                           player->isAdmin() ? "true" : "false",
                           QString::number(player->hp()),
                           QString::number(player->maxHp()),
                           QString::number(player->mp()),
                           QString::number(player->maxMp())));
    }
}
