#include "getpropcommand.h"

#include "engine/util.h"


GetPropCommand::GetPropCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Show the value of some object's property.\n"
                   "\n"
                   "Usage: get-prop <object-name> [#] <property-name>");
}

GetPropCommand::~GetPropCommand() {
}

void GetPropCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    GameObjectPtrList objects = takeObjects(currentArea()->objects());
    if (!requireUnique(objects, "Object not found.", "Object is not unique.")) {
        return;
    }

    QString propertyName = Util::toCamelCase(takeWord());

    if (propertyName == "id") {
        player()->send(QString::number(objects[0]->id()));
    } else {
        QVariant value = objects[0]->property(propertyName.toAscii().constData());
        if (!value.isValid()) {
            player()->send("Property is not set on object.");
            return;
        }

        switch (value.type()) {
            case QVariant::Bool:
                player()->send(value.toBool() ? "true" : "false");
                break;
            case QVariant::Int:
                player()->send(QString::number(value.toInt()));
                break;
            case QVariant::String:
                player()->send(value.toString());
                break;
            case QVariant::UserType:
                if (value.userType() == QMetaType::type("GameObjectPtr")) {
                    GameObjectPtr pointer = value.value<GameObjectPtr>();
                    if (pointer.isNull()) {
                        player()->send(pointer.toString());
                    } else {
                        player()->send(pointer.toString() + QString(" (%1)").arg(pointer->name()));
                    }
                    break;
                } else if (value.userType() == QMetaType::type("GameObjectPtrList")) {
                    QStringList strings;
                    foreach (GameObjectPtr pointer, value.value<GameObjectPtrList>()) {
                        strings << pointer.toString();
                    }
                    player()->send("[ " + strings.join(", ") + " ]");
                    break;
                } else if (value.userType() == QMetaType::type("CharacterStats")) {
                    CharacterStats stats = value.value<CharacterStats>();
                    player()->send(QString("[ %1, %2, %3, %4, %5, %6, %7, %8 ]")
                                   .arg(stats.strength).arg(stats.dexterity)
                                   .arg(stats.vitality).arg(stats.endurance)
                                   .arg(stats.intelligence).arg(stats.faith)
                                   .arg(stats.height).arg(stats.weight));
                    break;
                }
            default:
                player()->send("Cannot show property of unknown type.");
        }
    }
}
