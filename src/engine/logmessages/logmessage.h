#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include <QString>


class LogMessage {

    public:
        LogMessage();
        virtual ~LogMessage();

        virtual void process() = 0;
};

#endif // LOGMESSAGE_H
