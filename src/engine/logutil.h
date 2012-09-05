#ifndef LOGUTIL_H
#define LOGUTIL_H

#include <QString>


class LogUtil {

    public:
        static void LogCommand(const QString &playerName, const QString &command);
};

#endif // LOGUTIL_H
