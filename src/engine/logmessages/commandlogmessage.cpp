#include "commandlogmessage.h"

#include "diskutil.h"


CommandLogMessage::CommandLogMessage(const QString &playerName, const QString &command) :
    LogMessage(),
    m_playerName(playerName),
    m_command(command) {
}

CommandLogMessage::~CommandLogMessage() {
}

void CommandLogMessage::process() {

    DiskUtil::appendToLogFile("commands." + m_playerName, m_command);
}
