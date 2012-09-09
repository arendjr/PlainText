#include "areavisitstatslogmessage.h"

#include "diskutil.h"


AreaVisitStatsLogMessage::AreaVisitStatsLogMessage(const QString &identifier, int count) :
    LogMessage(),
    m_identifier(identifier),
    m_count(count) {
}

AreaVisitStatsLogMessage::~AreaVisitStatsLogMessage() {
}

void AreaVisitStatsLogMessage::log() {

    DiskUtil::appendToLogFile("areavisitstats", m_identifier.leftJustified(20) +
                                                QString::number(m_count));
}
