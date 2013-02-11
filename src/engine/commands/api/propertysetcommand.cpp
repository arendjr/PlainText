#include "propertysetcommand.h"

#include "realm.h"
#include "util.h"


#define super ApiCommand

PropertySetCommand::PropertySetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-property-set <request-id> <object-id> <property-name> <value>");
}

PropertySetCommand::~PropertySetCommand() {
}

void PropertySetCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = realm()->getObject(GameObjectType::Unknown, takeWord().toInt());
    if (object.isNull()) {
        sendError(404, "Object not found");
        return;
    }

    QString propertyName = Util::fullPropertyName(object.cast<GameObject *>(), takeWord());

    QString value = takeRest();

    QVariant variant = object->property(propertyName.toLatin1().constData());
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
        case QVariant::UserType: {
            MetaTypeRegistry::UserStringConverters converters =
                    MetaTypeRegistry::userStringConverters(QMetaType::typeName(variant.userType()));
            if (converters.userStringToTypeConverter) {
                try {
                    variant = converters.userStringToTypeConverter(value);
                    break;
                } catch (const GameException &exception) {
                    sendError(400, exception.what());
                    return;
                }
            }
        }   // fall-through
        default:
            sendError(400, "Invalid type");
            return;
    }

    object->setProperty(propertyName.toLatin1().constData(), variant);

    QVariantMap data;
    data["success"] = true;
    sendReply(data);
}
