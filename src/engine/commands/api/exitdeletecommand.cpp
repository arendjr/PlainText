#include "exitdeletecommand.h"

#include "realm.h"


#define super ApiCommand

ExitDeleteCommand::ExitDeleteCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-exit-delete <request-id> <exit-id>");
}

ExitDeleteCommand::~ExitDeleteCommand() {
}

void ExitDeleteCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    QString exitId = takeWord();
    GameObject *exit = realm()->getObject(GameObjectType::Exit, exitId.toUInt());

    if (exit == nullptr) {
        sendError(404, "Exit not found");
        return;
    }

    exit->setDeleted();

    QVariantMap data;
    data["success"] = true;
    sendReply(data);
}
