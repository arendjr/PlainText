#include "objectdeletecommand.h"

#include "realm.h"


#define super ApiCommand

ObjectDeleteCommand::ObjectDeleteCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-object-delete <request-id> <object-id>");
}

ObjectDeleteCommand::~ObjectDeleteCommand() {
}

void ObjectDeleteCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    QString objectId = takeWord();
    GameObject *object = realm()->getObject(GameObjectType::Unknown, objectId.toUInt());

    if (object == nullptr) {
        sendError(404, "Object not found");
        return;
    }

    object->setDeleted();

    QVariantMap data;
    data["success"] = true;
    sendReply(data);
}
