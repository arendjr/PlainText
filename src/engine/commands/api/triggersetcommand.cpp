#include "triggersetcommand.h"

#include "realm.h"
#include "util.h"


#define super ApiCommand

TriggerSetCommand::TriggerSetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-trigger-set <request-id> <object-id> <trigger-name> "
                   "<trigger-source>");
}

TriggerSetCommand::~TriggerSetCommand() {
}

void TriggerSetCommand::execute(Player *player, const QString &command) {

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

    QString source = takeRest();
    if (source.isEmpty()) {
        sendError(400, "Missing trigger source");
        return;
    }

    if (source.startsWith("function")) {
        source = "(" + source + ")";
    }

    ScriptFunction trigger;
    ScriptFunction::fromUserString(source, trigger);
    object->setTrigger(triggerName, trigger);

    sendReply(QString("Trigger %1 set.").arg(triggerName));

    if (triggerName == "onspawn") {
        object->killAllTimers();
        object->invokeTrigger(triggerName);
        send(QString("Respawn emulated for %1.")
             .arg(Util::highlight(QString("object #%1").arg(object->id()))));
    }
}
