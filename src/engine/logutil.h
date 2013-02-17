#ifndef LOGUTIL_H
#define LOGUTIL_H

#include <QObject>
#include <QString>


class QScriptValue;


class LogUtil : public QObject {

    Q_OBJECT

    public:
        static bool isLoggingEnabled();
        static void setLoggingEnabled(bool enabled);

        Q_INVOKABLE static void logDebug(const QString &message);
        Q_INVOKABLE static void logDebug(const QString &message, const QString &arg1);
        Q_INVOKABLE static void logDebug(const QString &message,
                                         const QString &arg1, const QString &arg2);
        Q_INVOKABLE static void logDebug(const QString &message,
                                         const QString &arg1, const QString &arg2,
                                         const QString &arg3);
        Q_INVOKABLE static void logDebug(const QString &message,
                                         const QString &arg1, const QString &arg2,
                                         const QString &arg3, const QString &arg4);

        Q_INVOKABLE static void logInfo(const QString &message);
        Q_INVOKABLE static void logInfo(const QString &message, const QString &arg1);
        Q_INVOKABLE static void logInfo(const QString &message,
                                        const QString &arg1, const QString &arg2);
        Q_INVOKABLE static void logInfo(const QString &message,
                                        const QString &arg1, const QString &arg2,
                                        const QString &arg3);
        Q_INVOKABLE static void logInfo(const QString &message,
                                        const QString &arg1, const QString &arg2,
                                        const QString &arg3, const QString &arg4);

        Q_INVOKABLE static void logError(const QString &message);
        Q_INVOKABLE static void logError(const QString &message, const QString &arg1);
        Q_INVOKABLE static void logError(const QString &message,
                                         const QString &arg1, const QString &arg2);
        Q_INVOKABLE static void logError(const QString &message,
                                         const QString &arg1, const QString &arg2,
                                         const QString &arg3);
        Q_INVOKABLE static void logError(const QString &message,
                                         const QString &arg1, const QString &arg2,
                                         const QString &arg3, const QString &arg4);

        Q_INVOKABLE static void logException(const QString &message, const QScriptValue &exception);
        Q_INVOKABLE static void logException(const QString &message, const QScriptValue &exception,
                                             const QString &arg2);

        Q_INVOKABLE static void logCommand(const QString &playerName, const QString &command);

        Q_INVOKABLE static void logNpcTalk(const QString &npcName, const QString &message);

        Q_INVOKABLE static void logSessionEvent(const QString &source, const QString &message);

        Q_INVOKABLE static void countRoomVisit(const QString &identifier, int count = 1);

        Q_INVOKABLE static void countPlayerDeath(const QString &identifier, int count = 1);

    private:
        static bool s_loggingEnabled;
};

#endif // LOGUTIL_H
