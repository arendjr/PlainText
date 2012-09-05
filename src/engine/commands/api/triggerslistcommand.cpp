#include "triggerslistcommand.h"

#include "scriptengine.h"


#define super ApiCommand

TriggersListCommand::TriggersListCommand(Player *player, QObject *parent) :
    super(player, parent) {

    setDescription("Syntax: api-triggers-list <request-id>");
}

TriggersListCommand::~TriggersListCommand() {
}

void TriggersListCommand::execute(const QString &command) {

    super::execute(command);

    sendReply(QStringList(ScriptEngine::triggers().keys()));
}
