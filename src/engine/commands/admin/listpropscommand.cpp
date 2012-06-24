#include "listpropscommand.h"

#include "engine/util.h"


ListPropsCommand::ListPropsCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Lists all properties of an object, with their values.\n"
                   "\n"
                   "Usage: list-props <object-name> [#]");
}

ListPropsCommand::~ListPropsCommand() {
}

void ListPropsCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    GameObjectPtrList objects = takeObjects(currentArea()->objects());
    if (!requireUnique(objects, "Object not found.", "Object is not unique.")) {
        return;
    }

    GameObjectPtr object = objects[0];
    player()->send(QString("These are all known properties of %1:\n"
                           "\n").arg(Util::highlight(QString("object #%1").arg(object->id()))));

    foreach (const QMetaProperty &metaProperty, object->storedMetaProperties()) {
        const char *name = metaProperty.name();
        QStringList valueLines;

        QVariant value = object->property(name);
        if (value.isValid()) {
            switch (value.type()) {
                case QVariant::Bool:
                    valueLines << (value.toBool() ? "true" : "false");
                    break;
                case QVariant::Int:
                    valueLines << QString::number(value.toInt());
                    break;
                case QVariant::String:
                    valueLines = Util::splitLines(value.toString(), 44);
                    break;
                case QVariant::UserType:
                    if (value.userType() == QMetaType::type("GameObjectPtr")) {
                        GameObjectPtr pointer = value.value<GameObjectPtr>();
                        if (pointer.isNull()) {
                            valueLines << "(not set)";
                        } else {
                            valueLines << pointer.toString() + QString(" (%1)").arg(pointer->name());
                        }
                        break;
                    } else if (value.userType() == QMetaType::type("GameObjectPtrList")) {
                        QStringList strings;
                        foreach (GameObjectPtr pointer, value.value<GameObjectPtrList>()) {
                            strings << pointer.toString();
                        }
                        valueLines = Util::splitLines("[ " + strings.join(", ") + " ]", 44);
                        break;
                    } else if (value.userType() == QMetaType::type("CharacterStats")) {
                        CharacterStats stats = value.value<CharacterStats>();
                        valueLines << QString("[ %1, %2, %3, %4, %5, %6, %7, %8 ]")
                                      .arg(stats.strength).arg(stats.dexterity)
                                      .arg(stats.vitality).arg(stats.endurance)
                                      .arg(stats.intelligence).arg(stats.faith)
                                      .arg(stats.height).arg(stats.weight);
                        break;
                    }
                default:
                    valueLines << "(unknown type)";
            }

            if (valueLines.size() == 0) {
                valueLines << "(empty string)";
            }
        } else {
            valueLines << "(not set)";
        }

        if (!metaProperty.isWritable()) {
            valueLines[valueLines.size() - 1].append(" (read-only)");
        }

        player()->send("  " + Util::highlight(QString(name).leftJustified(30)) +
                       "  " + valueLines[0]);
        for (int i = 1; i < valueLines.size(); i++) {
            player()->send("                                  " + valueLines[i]);
        }
    }

    player()->send("\n");
}
