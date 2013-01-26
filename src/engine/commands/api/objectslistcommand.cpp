#include "objectslistcommand.h"

#include "gameobject.h"
#include "realm.h"
#include "util.h"


#define super ApiCommand

ObjectsListCommand::ObjectsListCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-objects-list <request-id> <object-type>");
}

ObjectsListCommand::~ObjectsListCommand() {
}

void ObjectsListCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectType objectType = GameObjectType::fromString(Util::capitalize(takeWord()));
    if (objectType == GameObjectType::Unknown) {
        sendError(400, "Invalid object type");
        return;
    }

    QVector<GameObject *> objects = realm()->allObjects(objectType);
    QStringList data;
    for (GameObject *object : objects) {
        data.append(object->toJsonString());
    }
    sendReply(data);
}
