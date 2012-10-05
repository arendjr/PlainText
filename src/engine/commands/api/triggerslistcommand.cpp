#include "triggerslistcommand.h"

#include "scriptengine.h"


#define super ApiCommand

TriggersListCommand::TriggersListCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-triggers-list <request-id>");
}

TriggersListCommand::~TriggersListCommand() {
}

void TriggersListCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    sendReply(QStringList(ScriptEngine::triggers().keys()));
}
