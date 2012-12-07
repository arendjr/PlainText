#include "telnetserver.h"

#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

#include <QtIOCompressor>

#include "commandregistry.h"
#include "player.h"
#include "realm.h"
#include "session.h"


#define SE   "\xF0"
#define SB   "\xFA"
#define WILL "\xFB"
#define WONT "\xFC"
#define DO   "\xFD"
#define DONT "\xFE"
#define IAC  "\xFF"

#define MCCP "\x56"

#define MSDP             "\x45"
#define MSDP_VAR         "\x01"
#define MSDP_VAL         "\x02"
#define MSDP_TABLE_OPEN  "\x03"
#define MSDP_TABLE_CLOSE "\x04"
#define MSDP_ARRAY_OPEN  "\x05"
#define MSDP_ARRAY_CLOSE "\x06"

#define MSSP     "\x46"
#define MSSP_VAR "\x01"
#define MSSP_VAL "\x02"

#define BYTE(x) x[0]


Q_DECLARE_METATYPE(QtIOCompressor *)


TelnetServer::TelnetServer(Realm *realm, quint16 port, QObject *parent) :
    QObject(parent),
    m_realm(realm) {

    m_server = new QTcpServer(this);
    if (!m_server->listen(QHostAddress::Any, port)) {
        qDebug() << "Error: Can't launch telnet server";
    } else {
        qDebug() << "Telnet server is listening on port" << port;
    }

    connect(m_server, SIGNAL(newConnection()), this, SLOT(onClientConnected()));
}

TelnetServer::~TelnetServer() {
}

void TelnetServer::onClientConnected() {

    QTcpSocket *socket = m_server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(socket, SIGNAL(disconnected()), SLOT(onClientDisconnected()));

    Session *session = new Session(m_realm, "telnet", socket->peerAddress().toString(), socket);
    connect(session, SIGNAL(write(QString)), SLOT(onSessionOutput(QString)));
    connect(session, SIGNAL(terminate()), socket, SLOT(deleteLater()));

    session->open();

    socket->write(IAC WILL MCCP
                  IAC WILL MSDP
                  IAC WILL MSSP);

    socket->setProperty("session", QVariant::fromValue(session));
    socket->setProperty("compressor", QVariant::fromValue((QtIOCompressor *) nullptr));
    socket->setProperty("msdp", false);
    socket->setProperty("msdpSent", false);

    m_clients << socket;
}

void TelnetServer::onReadyRead() {

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) {
        return;
    }

    QByteArray buffer = socket->property("buffer").toByteArray();
    buffer.append(socket->readAll());

    for (int i = 0; i < buffer.length(); i++) {
        if (buffer[i] == '\n') {
            if (i > 0) {
                Session *session = socket->property("session").value<Session *>();
                session->onUserInput(buffer.left(buffer[i - 1] == '\r' ? i - 1 : i));
            }

            buffer.remove(0, i + 1);
            i--;
        } else if (i < buffer.length() - 1 && buffer[i] == BYTE(IAC)) {
            int length;
            switch (buffer[i + 1]) {
                case BYTE(IAC):
                    buffer.remove(i, 1);
                    break;
                case BYTE(SB):
                    length = buffer.indexOf(IAC SE) - i + 2;
                    if (length > 1) {
                        handleCommand(socket, buffer.mid(i, length));
                        buffer.remove(i, length);
                        i--;
                    }
                    break;
                case BYTE(WILL):
                case BYTE(WONT):
                case BYTE(DO):
                case BYTE(DONT):
                    if (i < buffer.length() - 2) {
                        handleCommand(socket, buffer.mid(i, 3));
                        buffer.remove(i, 3);
                        i--;
                    }
                    break;
                default:
                    handleCommand(socket, buffer.mid(i, 2));
                    buffer.remove(i, 2);
                    i--;
                    break;
            }
        }
    }

    socket->setProperty("buffer", buffer);
}

void TelnetServer::onClientDisconnected() {

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) {
        return;
    }

    m_clients.removeOne(socket);

    socket->deleteLater();
}

void TelnetServer::onSessionOutput(QString data) {

    if (data.trimmed().isEmpty()) {
        return;
    }

    Session *session = qobject_cast<Session *>(sender());
    if (!session) {
        return;
    }

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(session->parent());
    if (!socket) {
        return;
    }

    write(socket, data.replace("\n", "\r\n").toUtf8());

    if (session->authenticated()) {
        Player *player = session->player();
        Q_ASSERT(player);

        if (socket->property("msdp").toBool()) {
            if (!socket->property("msdpSent").toBool()) {
                sendMSDP(socket, player);
                socket->setProperty("msdpSent", true);
            }
            sendMSDPUpdate(socket, player);
        } else {
            write(socket, QString("(%1H %2M) ").arg(player->hp()).arg(player->mp()).toUtf8());
        }
    }
}

void TelnetServer::handleCommand(QTcpSocket *socket, const QByteArray &command) {

    switch (command[1]) {
        case BYTE(DO):
            if (command[2] == BYTE(MCCP)) {
                write(socket, IAC SB MCCP IAC SE);
                QtIOCompressor *compressor = new QtIOCompressor(socket);
                compressor->open(QIODevice::WriteOnly);
                socket->setProperty("compressor", QVariant::fromValue(compressor));
            } else if (command[2] == BYTE(MSDP)) {
                qDebug() << "Enabling MSDP...";
                socket->setProperty("msdp", true);
            } else if (command[2] == BYTE(MSSP)) {
                sendMSSP(socket);
            }
            break;
        case BYTE(DONT):
            if (command[2] == BYTE(MCCP)) {
                QtIOCompressor *compressor = socket->property("compressor")
                                             .value<QtIOCompressor *>();
                if (compressor) {
                    delete compressor;
                    compressor = nullptr;
                    socket->setProperty("compressor", QVariant::fromValue(compressor));
                }
            } else if (command[2] == BYTE(MSDP)) {
                socket->setProperty("msdp", false);
                socket->setProperty("msdpSent", false);
            }
            break;
        case BYTE(SB):
            if (command == IAC SB MSDP MSDP_VAR "LIST" MSDP_VAL "COMMANDS" IAC SE) {
                sendMSDPCommands(socket);
            }
            break;
        default:
            break;
    }
}

void TelnetServer::sendMSSP(QTcpSocket *socket) {

    QByteArray name = m_realm->name().toUtf8();
    QByteArray players = QByteArray::number(m_realm->onlinePlayers().length());
    QByteArray uptime = "-1";
    QByteArray crawlDelay = "-1";
    QByteArray port = QByteArray::number(m_server->serverPort());

    write(socket, IAC SB MSSP
                  MSSP_VAR "NAME" MSSP_VAL + name +
                  MSSP_VAR "PLAYERS" MSSP_VAL + players +
                  MSSP_VAR "UPTIME" MSSP_VAL + uptime +
                  MSSP_VAR "CRAWL DELAY" MSSP_VAL + crawlDelay +
                  MSSP_VAR "PORT" MSSP_VAL + port +
                  MSSP_VAR "CODEBASE" MSSP_VAL "PlainText"
                  MSSP_VAR "LANGUAGE" MSSP_VAL "English"
                  MSSP_VAR "FAMILY" MSSP_VAL "Custom"
                  MSSP_VAR "GENRE" MSSP_VAL "Fantasy"
                  MSSP_VAR "GAMEPLAY" MSSP_VAL "Adventure"
                  MSSP_VAR "GAMEPLAY" MSSP_VAL "Hack and Slash"
                  MSSP_VAR "GAMEPLAY" MSSP_VAL "Player versus Player"
                  MSSP_VAR "GAMEPLAY" MSSP_VAL "Player versus Environment"
                  MSSP_VAR "GAMEPLAY" MSSP_VAL "Roleplaying"
                  MSSP_VAR "ANSI" MSSP_VAL "1"
                  MSSP_VAR "GMCP" MSSP_VAL "0"
                  MSSP_VAR "MCCP" MSSP_VAL "1"
                  MSSP_VAR "MCP" MSSP_VAL "0"
                  MSSP_VAR "MSDP" MSSP_VAL "1"
                  MSSP_VAR "MSP" MSSP_VAL "0"
                  MSSP_VAR "MXP" MSSP_VAL "0"
                  MSSP_VAR "PUEBLO" MSSP_VAL "0"
                  MSSP_VAR "UTF-8" MSSP_VAL "1"
                  MSSP_VAR "VT100" MSSP_VAL "0"
                  MSSP_VAR "XTERM 256 COLORS" MSSP_VAL "0"
                  IAC SE);
}

void TelnetServer::sendMSDP(QTcpSocket *socket, Player *player) {

    QByteArray name = player->name().toUtf8();
    QByteArray serverId = m_realm->name().toUtf8();

    write(socket, IAC SB MSDP MSDP_VAR "ACCOUNT_NAME" MSDP_VAL + name + IAC SE
                  IAC SB MSDP MSDP_VAR "CHARACTER_NAME" MSDP_VAL + name + IAC SE
                  IAC SB MSDP MSDP_VAR "SERVER_ID" MSDP_VAL + serverId + IAC SE);
}

void TelnetServer::sendMSDPUpdate(QTcpSocket *socket, Player *player) {

    QByteArray health = QByteArray::number(player->hp());
    QByteArray healthMax = QByteArray::number(player->maxHp());
    QByteArray mana = QByteArray::number(player->mp());
    QByteArray manaMax = QByteArray::number(player->maxMp());
    QByteArray money = QByteArray::number(player->gold());

    write(socket, IAC SB MSDP MSDP_VAR "HEALTH" MSDP_VAL + health + IAC SE
                  IAC SB MSDP MSDP_VAR "HEALTH_MAX" MSDP_VAL + healthMax + IAC SE
                  IAC SB MSDP MSDP_VAR "MANA" MSDP_VAL + mana + IAC SE
                  IAC SB MSDP MSDP_VAR "MANA_MAX" MSDP_VAL + manaMax + IAC SE
                  IAC SB MSDP MSDP_VAR "MONEY" MSDP_VAL + money + IAC SE);
}

void TelnetServer::sendMSDPCommands(QTcpSocket *socket) {

    QByteArray commands = MSDP_ARRAY_OPEN;
    for (const QString &commandName : m_realm->commandRegistry()->commandNames()) {
        commands += MSDP_VAL + commandName;
    }
    commands += MSDP_ARRAY_CLOSE;

    write(socket, IAC SB MSDP MSDP_VAR "COMMANDS" MSDP_VAL + commands + IAC SE);
}

void TelnetServer::write(QTcpSocket *socket, const QByteArray &data) {

    QtIOCompressor *compressor = socket->property("compressor").value<QtIOCompressor *>();
    if (compressor) {
        compressor->write(data);
        compressor->flush();
    } else {
        socket->write(data);
    }
}
