#ifndef LOGUTIL_H
#define LOGUTIL_H

#include <QObject>
#include <QString>


class LogUtil : public QObject {

    Q_OBJECT

    public:
        Q_INVOKABLE static void logCommand(const QString &playerName, const QString &command);

        Q_INVOKABLE static void logNpcTalk(const QString &npcName, const QString &message);

        Q_INVOKABLE static void logSessionEvent(const QString &source, const QString &message);

        Q_INVOKABLE static void countRoomVisit(const QString &identifier, int count = 1);

        Q_INVOKABLE static void countPlayerDeath(const QString &identifier, int count = 1);
};

#endif // LOGUTIL_H
