#include "sessionlogmessage.h"

#include "diskutil.h"


SessionLogMessage::SessionLogMessage(const QString &source, const QString &message) :
    LogMessage(),
    m_source(source),
    m_message(message) {
}

SessionLogMessage::~SessionLogMessage() {
}

void SessionLogMessage::log() {

    DiskUtil::appendToLogFile("sessions", m_source.leftJustified(16) + m_message);
}
