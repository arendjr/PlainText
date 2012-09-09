#ifndef AREAVISITSTATSLOGMESSAGE_H
#define AREAVISITSTATSLOGMESSAGE_H

#include <QString>

#include "logmessage.h"


class AreaVisitStatsLogMessage : public LogMessage {

    public:
        AreaVisitStatsLogMessage(const QString &identifier, int count);
        virtual ~AreaVisitStatsLogMessage();

        virtual void log();

    private:
        QString m_identifier;
        int m_count;
};

#endif // AREAVISITSTATSLOGMESSAGE_H
