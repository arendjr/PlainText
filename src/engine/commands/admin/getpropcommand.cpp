#include "getpropcommand.h"

#include "engine/conversionutil.h"
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

    GameObjectPtr object = takeObject(currentArea()->objects());
    if (!requireSome(object, "Object not found.")) {
        return;
    }

    QString propertyName = Util::fullPropertyName(object.cast<GameObject *>(), takeWord());

    if (propertyName == "id") {
        send(QString::number(object->id()));
    } else {
        QVariant value = object->property(propertyName.toAscii().constData());
        send(ConversionUtil::toUserString(value));
    }
}
