#ifndef SESSIONLOGMESSAGE_H
#define SESSIONLOGMESSAGE_H

#include <QString>

#include "logmessage.h"


class SessionLogMessage : public LogMessage {

    public:
        SessionLogMessage(const QString &source, const QString &message);
        virtual ~SessionLogMessage();

        virtual void log();

    private:
        QString m_source;
        QString m_message;
};

#endif // SESSIONLOGMESSAGE_H
