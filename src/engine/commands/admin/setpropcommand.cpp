#include "setpropcommand.h"

#include "gameexception.h"
#include "metatyperegistry.h"
#include "util.h"


#define super AdminCommand

SetPropCommand::SetPropCommand(QObject *parent) :
    super(parent) {

    setDescription("Set the value of some object's property.\n"
                   "\n"
                   "Usage: set-prop <object-name> [#] <property-name> <value>");
}

SetPropCommand::~SetPropCommand() {
}

void SetPropCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = takeObject(currentRoom()->portals() + currentRoom()->characters() +
                                      currentRoom()->items());
    if (!requireSome(object, "Object not found.")) {
        return;
    }

    QString propertyName = Util::fullPropertyName(object.cast<GameObject *>(), takeWord());

    if (!assertWordsLeft("Usage: set-prop <object-name> [#] <property-name> <value>")) {
        return;
    }

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
        case QVariant::UserType: {
            MetaTypeRegistry::UserStringConverters converters =
                    MetaTypeRegistry::userStringConverters(QMetaType::typeName(variant.userType()));
            if (converters.userStringToTypeConverter) {
                try {
                    variant = converters.userStringToTypeConverter(value);
                    break;
                } catch (const GameException &exception) {
                    switch (exception.cause()) {
                        case GameException::InvalidCharacterStats:
                            send("Property of type CharacterStats takes the form [ <str>, <dex>, "
                                 "<vit>, <end>, <int>, <fai> ].");
                            break;
                        case GameException::InvalidPoint:
                            send("Property of type Point3D takes the form ( <x>, <y>, <z> ).");
                            break;
                        case GameException::InvalidVector:
                            send("Property of type Vector3D takes the form | <x>, <y>, <z> |.");
                            break;
                        default:
                            send(exception.what());
                            break;
                    }
                    return;
                }
            }
        }   // fall-through
        default:
            send("Setting property %1 is not supported.", propertyName);
            return;
    }

    if (variant.isValid()) {
        object->setProperty(propertyName.toAscii().constData(), variant);

        QVariant newValue = object->property(propertyName.toAscii().constData());
        send("Property %1 modified. New value: %2", propertyName,
             ConversionUtil::toUserString(newValue));

        if ((propertyName == "name" || propertyName == "plural" ||
             propertyName == "indefiniteArticle") && !object->plural().isEmpty()) {
            send(QString("New forms: %1 %2, one %3, two %4.").arg(object->indefiniteArticle(),
                                                                  object->name(),
                                                                  object->name(),
                                                                  object->plural()));
        }
    }
}

void SetPropCommand::setCommand(const QString &command) {

    static QRegExp whitespace("\\s+");

    if (command.contains(" description ")) {
        setWords(command.split(whitespace));
    } else {
        super::setCommand(command);
    }
}
