#ifndef COMMANDLOGMESSAGE_H
#define COMMANDLOGMESSAGE_H

#include <QString>

#include "logmessage.h"


class CommandLogMessage : public LogMessage {

    public:
        CommandLogMessage(const QString &playerName, const QString &command);
        virtual ~CommandLogMessage();

        virtual void process();

    private:
        QString m_playerName;
        QString m_command;
};

#endif // COMMANDLOGMESSAGE_H
