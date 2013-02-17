#include "logutil.h"

#include <iostream>

#include <QDebug>
#include <QScriptValue>

#include "commandlogmessage.h"
#include "errorlogmessage.h"
#include "npctalklogmessage.h"
#include "playerdeathstatslogmessage.h"
#include "realm.h"
#include "roomvisitstatslogmessage.h"
#include "sessionlogmessage.h"


bool LogUtil::s_loggingEnabled = false;


bool LogUtil::isLoggingEnabled() {

    return s_loggingEnabled;
}

void LogUtil::setLoggingEnabled(bool enabled) {

    s_loggingEnabled = enabled;
}

void LogUtil::logDebug(const QString &message) {

    QByteArray byteArray = message.toUtf8();
    qDebug() << byteArray.constData();
}

void LogUtil::logDebug(const QString &message, const QString &arg1) {

    QByteArray byteArray = message.arg(arg1).toUtf8();
    qDebug() << byteArray.constData();
}

void LogUtil::logDebug(const QString &message, const QString &arg1, const QString &arg2) {

    QByteArray byteArray = message.arg(arg1, arg2).toUtf8();
    qDebug() << byteArray.constData();
}

void LogUtil::logDebug(const QString &message, const QString &arg1, const QString &arg2,
                                               const QString &arg3) {

    QByteArray byteArray = message.arg(arg1, arg2, arg3).toUtf8();
    qDebug() << byteArray.constData();
}

void LogUtil::logDebug(const QString &message, const QString &arg1, const QString &arg2,
                                               const QString &arg3, const QString &arg4) {

    QByteArray byteArray = message.arg(arg1, arg2, arg3, arg4).toUtf8();
    qDebug() << byteArray.constData();
}

void LogUtil::logInfo(const QString &message) {

    QByteArray byteArray = message.toUtf8();
    std::cout << byteArray.constData() << std::endl;
}

void LogUtil::logInfo(const QString &message, const QString &arg1) {

    QByteArray byteArray = message.arg(arg1).toUtf8();
    std::cout << byteArray.constData() << std::endl;
}

void LogUtil::logInfo(const QString &message, const QString &arg1, const QString &arg2) {

    QByteArray byteArray = message.arg(arg1, arg2).toUtf8();
    std::cout << byteArray.constData() << std::endl;
}

void LogUtil::logInfo(const QString &message, const QString &arg1, const QString &arg2,
                                              const QString &arg3) {

    QByteArray byteArray = message.arg(arg1, arg2, arg3).toUtf8();
    std::cout << byteArray.constData() << std::endl;
}

void LogUtil::logInfo(const QString &message, const QString &arg1, const QString &arg2,
                                              const QString &arg3, const QString &arg4) {

    QByteArray byteArray = message.arg(arg1, arg2, arg3, arg4).toUtf8();
    std::cout << byteArray.constData() << std::endl;
}

void LogUtil::logError(const QString &message) {

    QByteArray byteArray = message.toUtf8();
    qWarning() << byteArray.constData();

    if (isLoggingEnabled()) {
        Realm::instance()->enqueueLogMessage(new ErrorLogMessage(message));
    }
}

void LogUtil::logError(const QString &message, const QString &arg1) {

    logError(message.arg(arg1));
}

void LogUtil::logError(const QString &message, const QString &arg1, const QString &arg2) {

    logError(message.arg(arg1, arg2));
}

void LogUtil::logError(const QString &message, const QString &arg1, const QString &arg2,
                                               const QString &arg3) {

    logError(message.arg(arg1, arg2, arg3));
}

void LogUtil::logError(const QString &message, const QString &arg1, const QString &arg2,
                                               const QString &arg3, const QString &arg4) {

    logError(message.arg(arg1, arg2, arg3, arg4));
}

void LogUtil::logException(const QString &message, const QScriptValue &exception) {

    QString arg1 = exception.toString();
    QScriptValue backtrace = exception.property("backtrace");

    logError(message.arg(arg1) +
             (backtrace.isValid() ? QString("\nBacktrace: %1").arg(backtrace.toString()) :
                                    QString("")));
}

void LogUtil::logException(const QString &message, const QScriptValue &exception,
                           const QString &arg2) {

    QString arg1 = exception.toString();
    QScriptValue backtrace = exception.property("backtrace");

    logError(message.arg(arg1, arg2) +
             (backtrace.isValid() ? QString("\nBacktrace: %1").arg(backtrace.toString()) :
                                    QString("")));
}

void LogUtil::logCommand(const QString &playerName, const QString &command) {

    if (isLoggingEnabled()) {
        Realm::instance()->enqueueLogMessage(new CommandLogMessage(playerName, command));
    }
}

void LogUtil::logNpcTalk(const QString &npcName, const QString &message) {

    if (isLoggingEnabled()) {
        Realm::instance()->enqueueLogMessage(new NpcTalkLogMessage(npcName, message));
    }
}

void LogUtil::logSessionEvent(const QString &source, const QString &message) {

    if (isLoggingEnabled()) {
        Realm::instance()->enqueueLogMessage(new SessionLogMessage(source, message));
    }
}

void LogUtil::countRoomVisit(const QString &identifier, int count) {

    if (isLoggingEnabled()) {
        Realm::instance()->enqueueLogMessage(new RoomVisitStatsLogMessage(identifier, count));
    }
}

void LogUtil::countPlayerDeath(const QString &identifier, int count) {

    if (isLoggingEnabled()) {
        Realm::instance()->enqueueLogMessage(new PlayerDeathStatsLogMessage(identifier, count));
    }
}
