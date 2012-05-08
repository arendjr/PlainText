#include "getpropcommand.h"


GetPropCommand::GetPropCommand(Character *character, QObject *parent) :
    AdminCommand(character, parent) {
}

GetPropCommand::~GetPropCommand() {
}

void GetPropCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    GameObjectPtrList objects = takeObjects(currentArea()->items() + currentArea()->exits());
    if (!requireUnique(objects, "Object not found.", "Object is not unique.")) {
        return;
    }

    QString propertyName = takeWord();

    if (propertyName == "id") {
        character()->send(QString::number(objects[0]->id()));
    } else {
        QVariant value = objects[0]->property(propertyName.toAscii().constData());
        switch (value.type()) {
            case QVariant::Bool:
                character()->send(value.toBool() ? "true" : "false");
                break;
            case QVariant::Int:
                character()->send(QString::number(value.toInt()));
                break;
            case QVariant::String:
                character()->send(value.toString());
                break;
            case QVariant::UserType:
                if (value.userType() == QMetaType::type("GameObjectPtr")) {
                    character()->send(value.value<GameObjectPtr>().toString());
                    break;
                } else if (value.userType() == QMetaType::type("GameObjectPtrList")) {
                    QStringList strings;
                    foreach (GameObjectPtr pointer, value.value<GameObjectPtrList>()) {
                        strings << pointer.toString();
                    }
                    character()->send("[ " + strings.join(", ") + " ]");
                    break;
                }
            default:
                character()->send("Cannot show property of unknown type.");
        }
    }
}
