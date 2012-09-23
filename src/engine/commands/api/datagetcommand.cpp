#include "datagetcommand.h"

#include "util.h"


#define super ApiCommand

DataGetCommand::DataGetCommand(Player *player, QObject *parent) :
    super(player, parent) {

    setDescription("Syntax: api-data-get <request-id> <object-id> <key>");
}

DataGetCommand::~DataGetCommand() {
}

void DataGetCommand::execute(const QString &command) {

    super::execute(command);

    GameObjectPtr object = takeObject(currentArea()->objects());
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
