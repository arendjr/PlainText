#include "areaslistcommand.h"

#include "realm.h"


#define super ApiCommand

AreasListCommand::AreasListCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-areas-list <request-id>");
}

AreasListCommand::~AreasListCommand() {
}

void AreasListCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    QVariantList data;
    for (const GameObjectPtr &area : realm()->allAreas()) {
        data << area->toJSON(IncludeId | DontIncludeTypeInfo);
    }
    sendReply(data);
}
