#include "datasetcommand.h"

#include "realm.h"
#include "util.h"


#define super ApiCommand

DataSetCommand::DataSetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-data-set <request-id> <object-id> <key> <type> <value>");
}

DataSetCommand::~DataSetCommand() {
}

void DataSetCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = realm()->getObject(GameObjectType::Unknown, takeWord().toInt());
    if (object.isNull()) {
        sendError(404, "Object not found");
        return;
    }

    QString key = takeWord();
    QString type = takeWord();
    QString value = takeRest();

    if (type == "bool") {
        object->setBoolData(key, value == "true");
    } else if (type == "int") {
        object->setIntData(key, value.toInt());
    } else if (type == "string") {
        object->setStringData(key, value);
    } else {
        sendError(400, "Invalid type");
        return;
    }

    QVariantMap data;
    data["success"] = true;
    sendReply(data);
}
