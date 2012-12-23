#include "triggerslistcommand.h"

#include "realm.h"
#include "scriptengine.h"
#include "triggerregistry.h"


#define super ApiCommand

TriggersListCommand::TriggersListCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-triggers-list <request-id>");
}

TriggersListCommand::~TriggersListCommand() {
}

void TriggersListCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    sendReply(realm()->triggerRegistry()->signatures());
}
