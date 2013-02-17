#include "errorlogmessage.h"

#include "diskutil.h"


ErrorLogMessage::ErrorLogMessage(const QString &message) :
    LogMessage(),
    m_message(message) {
}

ErrorLogMessage::~ErrorLogMessage() {
}

void ErrorLogMessage::log() {

    DiskUtil::appendToLogFile("errors", m_message);
}
