#include "setpropcommand.h"

#include "characterstats.h"
#include "point3d.h"
#include "util.h"
#include "vector3d.h"


#define super AdminCommand

SetPropCommand::SetPropCommand(QObject *parent) :
    super(parent) {

    setDescription("Set the value of some object's property.\n"
                   "\n"
                   "Usage: set-prop <object-name> [#] <property-name> <value>");
}

SetPropCommand::~SetPropCommand() {
}

void SetPropCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = takeObject(currentRoom()->objects());
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
        case QVariant::UserType:
            if (variant.userType() == QMetaType::type("CharacterStats")) {
                QStringList stringList = value.mid(1, value.length() - 2).split(',');
                if (stringList.length() == 6) {
                    CharacterStats stats;
                    stats.strength = stringList[0].toInt();
                    stats.dexterity = stringList[1].toInt();
                    stats.vitality = stringList[2].toInt();
                    stats.endurance = stringList[3].toInt();
                    stats.intelligence = stringList[4].toInt();
                    stats.faith = stringList[5].toInt();
                    variant = QVariant::fromValue(stats);
                } else {
                    send("Property of type CharacterStats takes the form [ <str>, <dex>, <vit>, "
                         "<end>, <int>, <fai> ].");
                    return;
                }
                break;
            } else if (variant.userType() == QMetaType::type("GameObjectPtr")) {
                variant = QVariant::fromValue(GameObjectPtr::fromString(realm(), value));
                break;
            } else if (variant.userType() == QMetaType::type("GameObjectPtrList")) {
                try {
                    GameObjectPtrList pointerList;
                    for (QString string : value.mid(1, value.length() - 2).split(',')) {
                        string = string.trimmed();
                        if (!string.isEmpty()) {
                            pointerList << GameObjectPtr::fromString(realm(), string);
                        }
                    }
                    variant = QVariant::fromValue(pointerList);
                } catch (const GameException &exception) {
                    send(exception.what());
                }
                break;
            } else if (variant.userType() == QMetaType::type("Point3D")) {
                QStringList stringList = value.mid(1, value.length() - 2).split(',');
                if (stringList.length() == 3) {
                    Point3D point(stringList[0].toInt(),
                                  stringList[1].toInt(),
                                  stringList[2].toInt());
                    variant = QVariant::fromValue(point);
                } else {
                    send("Property of type Point3D takes the form ( <x>, <y>, <z> ).");
                    return;
                }
                break;
            } else if (variant.userType() == QMetaType::type("Vector3D")) {
                QStringList stringList = value.mid(1, value.length() - 2).split(',');
                if (stringList.length() == 3) {
                    Vector3D vector(stringList[0].toInt(),
                                    stringList[1].toInt(),
                                    stringList[2].toInt());
                    variant = QVariant::fromValue(vector);
                } else {
                    send("Property of type Vector3D takes the form | <x>, <y>, <z> |.");
                    return;
                }
                break;
            }
        default:
            send("Setting property %1 is not supported.", propertyName);
            return;
    }

    if (variant.isValid()) {
        object->setProperty(propertyName.toAscii().constData(), variant);

        send("Property %1 modified.", propertyName);

        if (propertyName == "name" || propertyName == "plural" ||
            propertyName == "indefiniteArticle") {
            send(QString("New forms: %1 %2, one %3, two %4.").arg(object->indefiniteArticle(),
                                                                  object->name(),
                                                                  object->name(),
                                                                  object->plural()));
        }
    }
}
