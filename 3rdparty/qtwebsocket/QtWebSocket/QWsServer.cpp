#include "QWsServer.h"

#include <QStringList>
#include <QByteArray>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>

QWsServer::QWsServer(QObject * parent)
	: QObject(parent)
{
	tcpServer = new QTcpServer(this);
	connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newTcpConnection()));
	qsrand( QDateTime::currentMSecsSinceEpoch() );
}

QWsServer::~QWsServer()
{
	tcpServer->deleteLater();
}

bool QWsServer::listen(const QHostAddress & address, quint16 port)
{
	return tcpServer->listen(address, port);
}

void QWsServer::close()
{
	tcpServer->close();
}

QAbstractSocket::SocketError QWsServer::serverError()
{
	return tcpServer->serverError();
}

QString QWsServer::errorString()
{
	return tcpServer->errorString();
}

void QWsServer::newTcpConnection()
{
	QTcpSocket * clientSocket = tcpServer->nextPendingConnection();
	QObject * clientObject = qobject_cast<QObject*>(clientSocket);
	connect(clientObject, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}

void QWsServer::dataReceived()
{
	QTcpSocket * clientSocket = qobject_cast<QTcpSocket*>(sender());
	if (clientSocket == 0)
		return;

	while (true)
	{
		QStringList lines = QString(clientSocket->readAll()).split("\r\n");
		if ( lines.length() < 3 )
			break;

		QString requestLine = lines[0];
		QStringList elements = requestLine.split(' ');
		if ( elements.length() != 3 || elements[0] != "GET" || elements[2] != "HTTP/1.1" )
			break;

		// Extract datas

		QString resourceName = elements[1];
		if ( !resourceName.startsWith('/') )
			break;

		bool content = false;
		int version = 0,
		    hostPort = 0;
		QString hostAddress,
			key, key1, key2, key3,
			origin,
			protocol;

		int i;
		for ( i = 1; i < lines.length(); i++ )
		{
			QString line = lines[i];
			if ( content )
			{
				key3 = line;
				i++;
				break;
			}
			else
			{
				if ( line.isEmpty() )
				{
					content = true;
					continue;
				}

				QString fieldName = line.section(':', 0, 0);
				QString fieldValue = line.section(':', 1).trimmed();
				if ( fieldName.isEmpty() || fieldValue.isEmpty() )
					break;

				if ( fieldName == "Sec-WebSocket-Version" )
					version = fieldValue.toInt();
				else if ( fieldName == "Host" )
				{
					hostAddress = fieldValue.section(':', 0, 0);
					hostPort = fieldValue.section(':', 1).toInt();
				}
				else if ( fieldName == "Sec-WebSocket-Key" )
					key = fieldValue;
				else if ( fieldName == "Sec-WebSocket-Key1" )
					key1 = fieldValue;
				else if ( fieldName == "Sec-WebSocket-Key2" )
					key2 = fieldValue;
				else if ( fieldName == "Origin" || fieldName == "Sec-WebSocket-Origin" )
					origin = fieldValue;
				else if ( fieldName == "Sec-WebSocket-Protocol" )
					protocol = fieldValue;
			}
		}
		if ( i != lines.length() )
			break;

		////////////////////////////////////////////////////////////////////

		//if ( version < 6 )
		//{
		//	qDebug() << "======== Handshake Received \n"
		//			 << request
		//			 << "======== \n";
		//}

		// If the mandatory params are not setted, we abort the connection to the Websocket server
		if ( hostAddress.isEmpty()
			|| resourceName.isEmpty()
			|| ( version >= 6 && key.length() != 24 )
			|| ( version < 6 && ( key1.isEmpty() || key2.isEmpty() || key3.length() != 8 ) )
           )
			break;

		////////////////////////////////////////////////////////////////////

		// Compose handshake answer

		QString answer;

		QString accept;
		if ( version >= 6 )
		{
			accept = computeAcceptV2( key );
			answer.append("HTTP/1.1 101 Switching Protocols\r\n");
			answer.append("Upgrade: websocket\r\n");
			answer.append("Connection: Upgrade\r\n");
			answer.append("Sec-WebSocket-Accept: " + accept + "\r\n" + "\r\n");
		}
		else
        {
            accept = computeAcceptV1( key1, key2, key3 );
			answer.append("HTTP/1.1 101 WebSocket Protocol Handshake\r\n");
			answer.append("Upgrade: WebSocket\r\n");
			answer.append("Connection: Upgrade\r\n");
			answer.append("Sec-WebSocket-Origin: " + origin + "\r\n");
			answer.append("Sec-WebSocket-Location: ws://" + hostAddress + ( hostPort ? (":"+QString::number(hostPort)) : "" ) + resourceName + "\r\n");
			if ( !protocol.isEmpty() )
				answer.append("Sec-WebSocket-Protocol: " + protocol + "\r\n");
			answer.append("\r\n");
			answer.append( accept );
		}

		//if ( version < 6 )
		//{
		//	qDebug() << "======== Handshake sent \n"
		//			 << answer
		//			 << "======== \n";
		//}

		// Handshake OK, new connection
		disconnect(clientSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));

		// Send handshake answer
		clientSocket->write( answer.toAscii() );
		clientSocket->flush();

		// TEMPORARY CODE FOR LINUX COMPATIBILITY
		QWsSocket * wsSocket = new QWsSocket( clientSocket, version, this );
		addPendingConnection( wsSocket );
		emit newConnection();

		/*
		// ORIGINAL CODE
		int socketDescriptor = clientSocket->socketDescriptor();
		incomingConnection( socketDescriptor );
		*/
		return;
	}

	clientSocket->deleteLater();
}

void QWsServer::incomingConnection( int socketDescriptor )
{
	QTcpSocket * tcpSocket = new QTcpSocket(tcpServer);
	tcpSocket->setSocketDescriptor( socketDescriptor, QAbstractSocket::ConnectedState );
	QWsSocket * wsSocket = new QWsSocket( tcpSocket, 6, this );

	addPendingConnection( wsSocket );
	emit newConnection();
}

void QWsServer::addPendingConnection( QWsSocket * socket )
{
	if ( pendingConnections.size() < maxPendingConnections() )
		pendingConnections.enqueue(socket);
}

QWsSocket * QWsServer::nextPendingConnection()
{
	return pendingConnections.dequeue();
}

bool QWsServer::hasPendingConnections()
{
	if ( pendingConnections.size() > 0 )
		return true;
	return false;
}

int QWsServer::maxPendingConnections()
{
	return tcpServer->maxPendingConnections();
}

bool QWsServer::isListening()
{
	return tcpServer->isListening();
}

QNetworkProxy QWsServer::proxy()
{
	return tcpServer->proxy();
}

QHostAddress QWsServer::serverAddress()
{
	return tcpServer->serverAddress();
}

quint16 QWsServer::serverPort()
{
	return tcpServer->serverPort();
}

void QWsServer::setMaxPendingConnections( int numConnections )
{
	tcpServer->setMaxPendingConnections( numConnections );
}

void QWsServer::setProxy( const QNetworkProxy & networkProxy )
{
	tcpServer->setProxy( networkProxy );
}

bool QWsServer::setSocketDescriptor( int socketDescriptor )
{
	return tcpServer->setSocketDescriptor( socketDescriptor );
}

int QWsServer::socketDescriptor()
{
	return tcpServer->socketDescriptor();
}

bool QWsServer::waitForNewConnection( int msec, bool * timedOut )
{
	return tcpServer->waitForNewConnection( msec, timedOut );
}

QString QWsServer::computeAcceptV2(QString key)
{
	key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	QByteArray hash = QCryptographicHash::hash ( key.toUtf8(), QCryptographicHash::Sha1 );
	return hash.toBase64();
}

QString QWsServer::computeAcceptV1( QString key1, QString key2, QString key3 )
{
	QString numStr1;
	QString numStr2;
	int numSpaces1 = 0;
	int numSpaces2 = 0;

	QChar carac;
	for ( int i=0 ; i<key1.size() ; i++ )
	{
		carac = key1[ i ];
		if ( carac.isDigit() )
			numStr1.append( carac );
		else if ( carac == ' ' )
			numSpaces1++;
	}
	for ( int i=0 ; i<key2.size() ; i++ )
	{
		carac = key2[ i ];
		if ( carac.isDigit() )
			numStr2.append( carac );
		else if ( carac == ' ' )
			numSpaces2++;
	}

	quint32 num1 = numStr1.toUInt();
	quint32 num2 = numStr2.toUInt();

	//qDebug() << QString::number(num1);
	//qDebug() << QString::number(num2);

	//qDebug() << QString::number(numSpaces1);
	//qDebug() << QString::number(numSpaces2);

	num1 /= numSpaces1;
	num2 /= numSpaces2;

	QString concat = serializeInt( num1 ) + serializeInt( num2 ) + key3;

	QByteArray md5 = QCryptographicHash::hash( concat.toAscii(), QCryptographicHash::Md5 );
  
	return QString( md5 );
}

QString QWsServer::serializeInt( quint32 number, quint8 nbBytes )
{
	QString bin;
	quint8 currentNbBytes = 0;
	while (number > 0 && currentNbBytes < nbBytes)
	{  
		bin.prepend( QChar::fromAscii(number) );
		number = number >> 8;
		currentNbBytes++;
	}
	while (currentNbBytes < nbBytes)
	{
		bin.prepend( QChar::fromAscii(0) );
		currentNbBytes++;
	}
	return bin;
}
