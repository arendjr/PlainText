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
            QTextStream os(socket);
            os.setAutoDetectUnicode(true);

            if (tokens.length() < 2) {
                os << "HTTP/1.0 400 Bad Request\r\n"
                      "Content-Type: text/html; charset=\"utf-8\"\r\n"
                      "\r\n"
                      "<h1>Bad Request</h1>\n";
                socket->close();
                return;
            }

            QString path = tokens[1];
            if (!path.startsWith('/') || path.contains("/.")) {
                os << "HTTP/1.0 403 Forbidden\r\n"
                      "Content-Type: text/html; charset=\"utf-8\"\r\n"
                      "\r\n"
                      "<h1>Forbidden</h1>\n";
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

            QFile file(path);
            if (!file.open(QIODevice::ReadOnly)) {
                os << "HTTP/1.0 404 Not Found"
                      "Content-Type: text/html; charset=\"utf-8\"\r\n"
                      "\r\n"
                      "<h1>Not Found</h1>\n";
                socket->close();
                return;
            }

            QString content = file.readAll();

            QFileInfo info(path);
            if (info.baseName() == "__all__") {
                path = info.path();
                QString concatenated;
                for (const QString &fileName : content.split('\n', QString::SkipEmptyParts)) {
                    QFile file(path + "/" + fileName);
                    if (!file.open(QIODevice::ReadOnly)) {
                        concatenated = QString("Could not open %1").arg(file.fileName());
                        break;
                    }
                    concatenated.append(file.readAll() + "\n");
                }
                concatenated.append(QString("scriptLoaded(\"%1\");\n").arg(info.fileName()));
                content = concatenated;
            }

            QString mimeType = "text/html";
            if (info.suffix() == "js") {
                mimeType = "text/javascript";
            } else if (info.suffix() == "css") {
                mimeType = "text/css";
            }

            os << "HTTP/1.0 200 Ok\r\n"
                  "Content-Type: " << mimeType << "; charset=\"utf-8\"\r\n"
                  "\r\n"
               << content;

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
