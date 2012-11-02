#include "roomslistcommand.h"

#include "realm.h"


#define super ApiCommand

RoomsListCommand::RoomsListCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-rooms-list <request-id>");
}

RoomsListCommand::~RoomsListCommand() {
}

void RoomsListCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    QVariantList data;
    for (const GameObjectPtr &room : realm()->allRooms()) {
        data << room->toJsonString();
    }
    sendReply(data);
}
