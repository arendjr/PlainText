#include "exitslistcommand.h"

#include "realm.h"


#define super ApiCommand

ExitsListCommand::ExitsListCommand(Player *player, QObject *parent) :
    super(player, parent) {

    setDescription("Syntax: api-exits-list <request-id>");
}

ExitsListCommand::~ExitsListCommand() {
}

void ExitsListCommand::execute(const QString &command) {

    super::execute(command);

    QVariantList data;
    for (const GameObjectPtr &exit : realm()->allExits()) {
        data << QVariant::fromValue(exit.cast<GameObject *>());
    }
    sendReply(data);
}
