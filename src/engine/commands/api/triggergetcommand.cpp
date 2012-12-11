#include "triggergetcommand.h"

#include "realm.h"


#define super ApiCommand

TriggerGetCommand::TriggerGetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-trigger-get <request-id> <object-id> <trigger-name>");
}

TriggerGetCommand::~TriggerGetCommand() {
}

void TriggerGetCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = realm()->getObject(GameObjectType::Unknown, takeWord().toInt());
    if (object.isNull()) {
        sendError(404, "Object not found");
        return;
    }

    QString triggerName = takeWord().toLower();
    if (!triggerName.startsWith("on")) {
        sendError(400, "Invalid trigger name");
        return;
    }

    ScriptFunction trigger = object->trigger(triggerName);

    QVariantMap data;
    data["id"] = object->id();
    data["triggerSource"] = trigger.source;
    sendReply(data);
}
