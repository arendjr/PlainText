#include "datagetcommand.h"

#include "realm.h"
#include "util.h"


#define super ApiCommand

DataGetCommand::DataGetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-data-get <request-id> <object-id> <key>");
}

DataGetCommand::~DataGetCommand() {
}

void DataGetCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = realm()->getObject(GameObjectType::Unknown, takeWord().toInt());
    if (object.isNull()) {
        sendError(404, "Object not found");
        return;
    }

    QString key = takeWord();

    QVariantMap data;
    data["id"] = object->id();
    data[key] = object->data()[key];
    sendReply(data);
}
