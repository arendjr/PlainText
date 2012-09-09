#ifndef LOGUTIL_H
#define LOGUTIL_H

#include <QString>


class LogUtil {

    public:
        static void logCommand(const QString &playerName, const QString &command);

        static void logNpcTalk(const QString &npcName, const QString &message);

        static void logSessionEvent(const QString &source, const QString &message);

        static void countAreaVisit(const QString &identifier, int count = 1);

        static void countPlayerDeath(const QString &identifier, int count = 1);
};

#endif // LOGUTIL_H
