#include "commandevent.h"

#include <QDebug>

#include "commandinterpreter.h"


CommandEvent::CommandEvent(CommandInterpreter *interpreter, const QString &command) :
    Event(),
    m_interpreter(interpreter),
    m_command(command) {
}

CommandEvent::~CommandEvent() {
}

void CommandEvent::process() {

    if (!m_interpreter || m_command.isEmpty()) {
        qWarning() << "Processing uninitialized command event. Skipped.";
        return;
    }

    m_interpreter->execute(m_command);
}

QString CommandEvent::toString() const {

    return "Command: " + m_command;
}
