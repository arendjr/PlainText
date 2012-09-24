#ifndef RETRIEVESTATSLOGMESSAGE_H
#define RETRIEVESTATSLOGMESSAGE_H

#include <QString>

#include "logmessage.h"


class Player;

class RetrieveStatsLogMessage : public LogMessage {

    public:
        RetrieveStatsLogMessage(Player *recipient, const QString &requestId,
                                const QString &type, int numDays);
        virtual ~RetrieveStatsLogMessage();

        virtual void log();

    private:
        Player *m_recipient;
        QString m_requestId;
        QString m_type;
        int m_numDays;
};

#endif // RETRIEVESTATSLOGMESSAGE_H
