#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include <QString>


class LogMessage {

    public:
        LogMessage();
        virtual ~LogMessage();

        virtual void log() = 0;
};

#endif // LOGMESSAGE_H
