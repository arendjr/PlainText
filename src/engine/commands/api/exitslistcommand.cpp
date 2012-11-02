#include "exitslistcommand.h"

#include "realm.h"


#define super ApiCommand

ExitsListCommand::ExitsListCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-exits-list <request-id>");
}

ExitsListCommand::~ExitsListCommand() {
}

void ExitsListCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    QVariantList data;
    for (const GameObjectPtr &exit : realm()->allExits()) {
        data << exit->toJsonString();
    }
    sendReply(data);
}
