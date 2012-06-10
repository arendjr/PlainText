#include "setpropcommand.h"

#include "engine/characterstats.h"
#include "engine/item.h"
#include "engine/util.h"


SetPropCommand::SetPropCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Set the value of some object's property.\n"
                   "\n"
                   "Usage: set-prop <object-name> [#] <property-name> <value>");
}

SetPropCommand::~SetPropCommand() {
}

void SetPropCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    GameObjectPtrList objects = takeObjects(currentArea()->objects());
    if (!requireUnique(objects, "Object not found.", "Object is not unique.")) {
        return;
    }

    QString propertyName = Util::toCamelCase(takeWord());

    if (!assertWordsLeft("Usage: set-prop <object-name> [#] <property-name> <value>")) {
        return;
    }

    QString value = takeRest();

    QVariant variant = objects[0]->property(propertyName.toAscii().constData());
    switch (variant.type()) {
        case QVariant::Bool:
            variant = (value == "true");
            break;
        case QVariant::Int:
            variant = value.toInt();
            break;
        case QVariant::String:
            variant = value.replace("\\n", "\n");
            break;
        case QVariant::UserType:
            if (variant.userType() == QMetaType::type("GameObjectPtr")) {
                variant = QVariant::fromValue(GameObjectPtr::fromString(value));
                break;
            } else if (variant.userType() == QMetaType::type("GameObjectPtrList")) {
                try {
                    GameObjectPtrList pointerList;
                    foreach (const QString &string, value.split(' ')) {
                        pointerList << GameObjectPtr::fromString(string);
                    }
                    variant = QVariant::fromValue(pointerList);
                } catch (const GameException &exception) {
                    send(exception.what());
                }
                break;
            } else if (variant.userType() == QMetaType::type("CharacterStats")) {
                QStringList stringList = value.mid(1, value.length() - 2).split(',');
                if (stringList.length() == 8) {
                    CharacterStats stats;
                    stats.strength = stringList[0].toInt();
                    stats.dexterity = stringList[1].toInt();
                    stats.vitality = stringList[2].toInt();
                    stats.endurance = stringList[3].toInt();
                    stats.intelligence = stringList[4].toInt();
                    stats.faith = stringList[5].toInt();
                    stats.height = stringList[6].toInt();
                    stats.weight = stringList[7].toInt();
                    variant = QVariant::fromValue(stats);
                } else {
                    send("Property of type CharacterStats takes the form [ <str>, <dex>, <vit>, <end>, <int>, <fai>, <hei>, <wei> ].");
                    return;
                }
                break;
            }
        default:
            send(QString("Setting property %1 is not supported.").arg(propertyName));
            return;
    }

    if (variant.isValid()) {
        objects[0]->setProperty(propertyName.toAscii().constData(), variant);

        send(QString("Property %1 modified.").arg(propertyName));

        try {
            Item *item = objects[0].cast<Item *>();
            if (propertyName == "name" || propertyName == "plural" || propertyName == "indefiniteArticle") {
                send(QString("New forms: one %1, two %2, %3 %4.").arg(item->name(), item->plural(),
                                                                      item->indefiniteArticle(), item->name()));
            }
        } catch (const GameException &exception) {
            Q_UNUSED(exception);
        }
    }
}
