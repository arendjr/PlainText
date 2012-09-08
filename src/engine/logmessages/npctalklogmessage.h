#ifndef NPCTALKLOGMESSAGE_H
#define NPCTALKLOGMESSAGE_H

#include <QString>

#include "logmessage.h"


class NpcTalkLogMessage : public LogMessage {

    public:
        NpcTalkLogMessage(const QString &npcName, const QString &message);
        virtual ~NpcTalkLogMessage();

        virtual void process();

    private:
        QString m_npcName;
        QString m_message;
};

#endif // NPCTALKLOGMESSAGE_H
