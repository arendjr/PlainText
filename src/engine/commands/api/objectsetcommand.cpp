#include "objectsetcommand.h"

#include "conversionutil.h"
#include "realm.h"


#define super ApiCommand

ObjectSetCommand::ObjectSetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-object-set <request-id> <object-type> <object-id> <object-JSON>");
}

ObjectSetCommand::~ObjectSetCommand() {
}

void ObjectSetCommand::execute(Character *character, const QString &command) {

    super::prepareExecute(character, command);

    GameObjectType objectType = GameObjectType::fromString(takeWord());
    QString objectId = takeWord();

    if (objectType == GameObjectType::Unknown) {
        sendError(400, "Invalid object type");
        return;
    }

    GameObject *object;
    if (objectId == "new") {
        object = GameObject::createByObjectType(realm(), objectType);
    } else {
        if (objectId.toUInt() == 0 && objectType != GameObjectType::Realm) {
            sendError(400, "Invalid object ID");
            return;
        }

        object = realm()->getObject(GameObjectType::Unknown, objectId.toUInt());
        if (object) {
            if (object->objectType() != objectType) {
                sendError(400, QString("Invalid type for object with ID %1").arg(objectId));
                return;
            }
        } else {
            object = GameObject::createByObjectType(realm(), objectType, objectId.toUInt());
        }
    }

    object->loadJson(takeRest());

    QVariantMap data;
    data["success"] = true;
    data["object"] = object->toJsonString();
    sendReply(data);
}

void ObjectSetCommand::setCommand(const QString &command) {

    static QRegExp whitespace("\\s+");

    setWords(command.split(whitespace));
}
