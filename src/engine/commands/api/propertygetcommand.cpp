#include "propertygetcommand.h"

#include "realm.h"
#include "util.h"


#define super ApiCommand

PropertyGetCommand::PropertyGetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-property-get <request-id> <object-id> <property-name>");
}

PropertyGetCommand::~PropertyGetCommand() {
}

void PropertyGetCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object;
    if (peekWord().toInt()) {
        object = realm()->getObject(GameObjectType::Unknown, takeWord().toInt());
    } else {
        object = takeObject(currentRoom()->characters() + currentRoom()->items());
    }

    if (object.isNull()) {
        sendError(404, "Object not found");
        return;
    }

    QString propertyName = Util::fullPropertyName(object.cast<GameObject *>(), takeWord());

    const QMetaObject *metaObject = object->metaObject();
    int propertyIndex = metaObject->indexOfProperty(propertyName.toAscii().constData());
    if (propertyIndex == -1) {
        sendError(400, "Unknown property name");
        return;
    }

    QMetaProperty metaProperty = metaObject->property(propertyIndex);

    QVariantMap data;
    data["id"] = object->id();
    data["propertyName"] = propertyName;
    data["readOnly"] = !metaProperty.isStored();
    data[propertyName] = object->property(propertyName.toAscii().constData());
    sendReply(data);
}
