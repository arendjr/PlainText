#ifndef ERRORLOGMESSAGE_H
#define ERRORLOGMESSAGE_H

#include <QString>

#include "logmessage.h"


class ErrorLogMessage : public LogMessage {

    public:
        ErrorLogMessage(const QString &message);
        virtual ~ErrorLogMessage();

        virtual void log();

    private:
        QString m_source;
        QString m_message;
};

#endif // ERRORLOGMESSAGE_H
