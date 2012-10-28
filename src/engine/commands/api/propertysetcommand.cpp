#include "propertysetcommand.h"

#include "util.h"


#define super ApiCommand

PropertySetCommand::PropertySetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-property-set <request-id> <object-id> <property-name> <value>");
}

PropertySetCommand::~PropertySetCommand() {
}

void PropertySetCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = takeObject(currentRoom()->objects());
    if (object.isNull()) {
        sendError(404, "Object not found");
        return;
    }

    QString propertyName = Util::fullPropertyName(object.cast<GameObject *>(), takeWord());

    QString value = takeRest();

    QVariant variant = object->property(propertyName.toAscii().constData());
    switch (variant.type()) {
        case QVariant::Bool:
            variant = (value == "true");
            break;
        case QVariant::Int:
            variant = value.toInt();
            break;
        case QVariant::Double:
            variant = value.toDouble();
            break;
        case QVariant::String:
            variant = value.replace("\\n", "\n");
            break;
        case QVariant::UserType:
            if (variant.userType() == QMetaType::type("Point")) {
                QStringList stringList = value.mid(1, value.length() - 2).split(',');
                if (stringList.length() == 3) {
                    Point3D point(stringList[0].toInt(),
                                stringList[1].toInt(),
                                stringList[2].toInt());
                    variant = QVariant::fromValue(point);
                } else {
                    sendError(400, "Invalid format for value of type Point.");
                    return;
                }
                break;
            }
        default:
            sendError(400, "Invalid type");
            return;
    }

    object->setProperty(propertyName.toAscii().constData(), variant);

    QVariantMap data;
    data["success"] = true;
    sendReply(data);
}
