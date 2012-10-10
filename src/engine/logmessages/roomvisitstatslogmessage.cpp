#include "roomvisitstatslogmessage.h"

#include "diskutil.h"


RoomVisitStatsLogMessage::RoomVisitStatsLogMessage(const QString &identifier, int count) :
    LogMessage(),
    m_identifier(identifier),
    m_count(count) {
}

RoomVisitStatsLogMessage::~RoomVisitStatsLogMessage() {
}

void RoomVisitStatsLogMessage::log() {

    DiskUtil::appendToLogFile("roomvisitstats", m_identifier.leftJustified(20) +
                                                QString::number(m_count));
}
