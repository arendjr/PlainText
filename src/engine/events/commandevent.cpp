#include "commandevent.h"

#include "logutil.h"
#include "player.h"


CommandEvent::CommandEvent(Character *player, const QString &command) :
    Event(),
    m_player(player),
    m_command(command) {
}

CommandEvent::~CommandEvent() {
}

void CommandEvent::process() {

    if (!m_player || m_command.isEmpty()) {
        LogUtil::logDebug("Processing uninitialized command event. Skipped.");
        return;
    }

    m_player->execute(m_command);
}

QString CommandEvent::toString() const {

    return "Command: " + m_command;
}
