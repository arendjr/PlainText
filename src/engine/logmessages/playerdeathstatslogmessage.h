#ifndef PLAYERDEATHSTATSLOGMESSAGE_H
#define PLAYERDEATHSTATSLOGMESSAGE_H

#include <QString>

#include "logmessage.h"


class PlayerDeathStatsLogMessage : public LogMessage {

    public:
        PlayerDeathStatsLogMessage(const QString &identifier, int count);
        virtual ~PlayerDeathStatsLogMessage();

        virtual void log();

    private:
        QString m_identifier;
        int m_count;
};

#endif // PLAYERDEATHSTATSLOGMESSAGE_H
