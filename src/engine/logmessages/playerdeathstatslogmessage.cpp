#include "playerdeathstatslogmessage.h"

#include "diskutil.h"


PlayerDeathStatsLogMessage::PlayerDeathStatsLogMessage(const QString &identifier, int count) :
    LogMessage(),
    m_identifier(identifier),
    m_count(count) {
}

PlayerDeathStatsLogMessage::~PlayerDeathStatsLogMessage() {
}

void PlayerDeathStatsLogMessage::log() {

    DiskUtil::appendToLogFile("playerdeathstats", m_identifier.leftJustified(20) +
                                                  QString::number(m_count));
}
