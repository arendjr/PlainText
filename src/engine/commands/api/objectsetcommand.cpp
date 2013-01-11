#include "objectsetcommand.h"

#include "conversionutil.h"
#include "realm.h"


#define super ApiCommand

ObjectSetCommand::ObjectSetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-object-set <request-id> <object-JSON>");
}

ObjectSetCommand::~ObjectSetCommand() {
}

void ObjectSetCommand::execute(Character *character, const QString &command) {

    super::prepareExecute(character, command);



    QVariantMap data;
    data["success"] = true;
    data["object"] = object->toJsonString();
    sendReply(data);
}
