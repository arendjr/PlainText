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

    GameObjectPtrList objects = takeObjects(currentArea()->objects());
    if (!requireUnique(objects, "Object not found.", "Object is not unique.")) {
        return;
    }

    QString propertyName = Util::toCamelCase(takeWord());

    if (propertyName == "id") {
        send(QString::number(objects[0]->id()));
    } else {
        QVariant value = objects[0]->property(propertyName.toAscii().constData());
        send(ConversionUtil::toUserString(value));
    }
}
