#ifndef ROOMVISITSTATSLOGMESSAGE_H
#define ROOMVISITSTATSLOGMESSAGE_H

#include <QString>

#include "logmessage.h"


class RoomVisitStatsLogMessage : public LogMessage {

    public:
        RoomVisitStatsLogMessage(const QString &identifier, int count);
        virtual ~RoomVisitStatsLogMessage();

        virtual void log();

    private:
        QString m_identifier;
        int m_count;
};

#endif // ROOMVISITSTATSLOGMESSAGE_H
