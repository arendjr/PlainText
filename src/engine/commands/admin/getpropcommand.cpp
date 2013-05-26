#include "getpropcommand.h"

#include "conversionutil.h"
#include "scriptengine.h"
#include "util.h"


#define super AdminCommand

GetPropCommand::GetPropCommand(QObject *parent) :
    super(parent) {

    setDescription("Show the value of some object's property.\n"
                   "\n"
                   "Usage: get-prop <object-name> [#] <property-name>");
}

GetPropCommand::~GetPropCommand() {
}

void GetPropCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = takeObject(currentRoom()->portals() + currentRoom()->characters() +
                                      currentRoom()->items());
    if (!requireSome(object, "Object not found.")) {
        return;
    }

    QString propName = takeWord();
    QString propertyName = Util::fullPropertyName(object.cast<GameObject *>(), propName);
    if (propertyName.isEmpty()) {
        if (propName.isEmpty()) {
            send("Usage: get-prop <object-name> [#] <property-name>");
        } else {
            propertyName = ConversionUtil::jsString(propName);
            ScriptEngine *scriptEngine = ScriptEngine::instance();
            QScriptValue value = scriptEngine->evaluate(QString("$('%1:%2')[%3]")
                                                        .arg(object->objectType().toString())
                                                        .arg(object->id())
                                                        .arg(propertyName));
            if (value.isUndefined()) {
                send(QString("Unknown property: %1").arg(propName));
            } else {
                send(value.toString());
            }
        }
        return;
    }

    if (propertyName == "id") {
        send(QString::number(object->id()));
    } else {
        QVariant value = object->property(propertyName.toLatin1().constData());
        send(ConversionUtil::toUserString(value));
    }
}
