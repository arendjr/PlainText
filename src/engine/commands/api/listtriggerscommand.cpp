#include "listtriggerscommand.h"

#include "engine/realm.h"
#include "engine/scriptengine.h"


#define super ApiCommand

ListTriggersCommand::ListTriggersCommand(Player *player, QObject *parent) :
    super(player, parent) {
}

ListTriggersCommand::~ListTriggersCommand() {
}

void ListTriggersCommand::execute(const QString &command) {

    super::execute(command);

    sendReply(QStringList(ScriptEngine::triggers().keys()));
}
