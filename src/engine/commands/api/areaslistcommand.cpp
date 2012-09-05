#include "areaslistcommand.h"

#include "realm.h"


#define super ApiCommand

AreasListCommand::AreasListCommand(Player *player, QObject *parent) :
    super(player, parent) {

    setDescription("Syntax: api-areas-list <request-id>");
}

AreasListCommand::~AreasListCommand() {
}

void AreasListCommand::execute(const QString &command) {

    super::execute(command);

    QVariantList data;
    for (const GameObjectPtr &area : realm()->allAreas()) {
        data << QVariant::fromValue(area.cast<GameObject *>());
    }
    sendReply(data);
}
