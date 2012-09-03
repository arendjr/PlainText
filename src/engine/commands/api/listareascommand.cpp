#include "listareascommand.h"

#include "engine/realm.h"


#define super ApiCommand

ListAreasCommand::ListAreasCommand(Player *player, QObject *parent) :
    super(player, parent) {
}

ListAreasCommand::~ListAreasCommand() {
}

void ListAreasCommand::execute(const QString &command) {

    super::execute(command);

    QVariantList data;
    for (const GameObjectPtr &area : realm()->allAreas()) {
        data << QVariant::fromValue(area.cast<GameObject *>());
    }
    sendReply(data);
}
