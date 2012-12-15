#include "httpserver.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QStringList>
#include <QTcpSocket>


HttpServer::HttpServer(quint16 port, QObject *parent) :
    QTcpServer(parent) {

    if (!listen(QHostAddress::Any, port)) {
        qDebug() << "Error: Can't launch HTTP server";
    } else {
        qDebug() << "HTTP server is listening on port" << port;
    }
}

HttpServer::~HttpServer() {
}

void HttpServer::incomingConnection(int socketDescriptor) {

    QTcpSocket *socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(socket, SIGNAL(disconnected()), SLOT(onDisconnected()));
    socket->setSocketDescriptor(socketDescriptor);
}

void HttpServer::onReadyRead() {

    QTcpSocket *socket = (QTcpSocket *) sender();
    if (socket->canReadLine()) {
        QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n]+"));
        if (tokens[0] == "GET") {
            if (tokens.length() < 2) {
                socket->write("HTTP/1.0 400 Bad Request\r\n"
                              "Content-Type: text/html; charset=\"utf-8\"\r\n"
                              "\r\n"
                              "<h1>Bad Request</h1>\n");
                socket->close();
                return;
            }

            QString path = tokens[1];
            if (!path.startsWith('/') || path.contains("/.")) {
                socket->write("HTTP/1.0 403 Forbidden\r\n"
                              "Content-Type: text/html; charset=\"utf-8\"\r\n"
                              "\r\n"
                              "<h1>Forbidden</h1>\n");
                socket->close();
                return;
            }

            if (path.contains('?')) {
                path = path.section('?', 0, 0);
            }
            if (path == "/") {
                path = "/index.html";
            }
            path = "../web" + path;

            if (path.contains("/js/")) {
                QString minifiedPath = path;
                minifiedPath.replace("/js/", "/min/");
                if (QFile::exists(minifiedPath)) {
                    path = minifiedPath;
                }
            }

            QFile file(path);
            if (!file.open(QIODevice::ReadOnly)) {
                socket->write("HTTP/1.0 404 Not Found"
                              "Content-Type: text/html; charset=\"utf-8\"\r\n"
                              "\r\n"
                              "<h1>Not Found</h1>\n");
                socket->close();
                return;
            }

            QByteArray content = file.readAll();

            QFileInfo info(path);
            QString mimeType = "text/html";
            QString charset;
            if (info.suffix() == "js") {
                mimeType = "text/javascript";
            } else if (info.suffix() == "png") {
                mimeType = "image/png";
            } else if (info.suffix() == "css") {
                mimeType = "text/css";
            } else {
                charset = "; charset=\"utf-8\"";
            }

            socket->write(QString("HTTP/1.0 200 Ok\r\n"
                                  "Content-Type: %1%2\r\n"
                                  "Content-Length: %3\r\n"
                                  "\r\n")
                          .arg(mimeType, charset, QString::number(content.length()))
                          .toLatin1() +
                          content);
            socket->close();

            if (socket->state() == QTcpSocket::UnconnectedState) {
                delete socket;
            }
        }
    }
}

void HttpServer::onDisconnected() {

    sender()->deleteLater();
}
