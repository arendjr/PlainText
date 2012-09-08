#include "npctalklogmessage.h"

#include "diskutil.h"


NpcTalkLogMessage::NpcTalkLogMessage(const QString &npcName, const QString &message) :
    LogMessage(),
    m_npcName(npcName),
    m_message(message) {
}

NpcTalkLogMessage::~NpcTalkLogMessage() {
}

void NpcTalkLogMessage::process() {

    DiskUtil::appendToLogFile("npctalk." + m_npcName,
                              m_message.isEmpty() ? "(no message)" : m_message);
}
