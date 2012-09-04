#include "listexitscommand.h"

#include "engine/realm.h"


#define super ApiCommand

ListExitsCommand::ListExitsCommand(Player *player, QObject *parent) :
    super(player, parent) {
}

ListExitsCommand::~ListExitsCommand() {
}

void ListExitsCommand::execute(const QString &command) {

    super::execute(command);

    QVariantList data;
    for (const GameObjectPtr &exit : realm()->allExits()) {
        data << QVariant::fromValue(exit.cast<GameObject *>());
    }
    sendReply(data);
}
