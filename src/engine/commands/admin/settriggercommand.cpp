#include "settriggercommand.h"

#include "util.h"


#define super AdminCommand

SetTriggerCommand::SetTriggerCommand(QObject *parent) :
    super(parent) {

    setDescription("Set the script for some object's trigger.\n"
                   "\n"
                   "Usage: set-trigger <object-name> [#] <trigger-name> <script>\n"
                   "\n"
                   "Type *help triggers* to see a list of all available triggers.");
}

SetTriggerCommand::~SetTriggerCommand() {
}

void SetTriggerCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = takeObject(currentRoom()->objects());
    if (!requireSome(object, "Object not found.")) {
        return;
    }

    QString triggerName = takeWord().toLower();

    if (!triggerName.startsWith("on")) {
        send("Not a valid trigger name.");
        return;
    }

    QString source = takeRest();
    if (source.isEmpty()) {
        send("No source for trigger.");
        return;
    }

    if (source.startsWith("function")) {
        source = "(" + source + ")";
    }

    ScriptFunction trigger;
    ScriptFunction::fromUserString(source, trigger);
    object->setTrigger(triggerName, trigger);
    send(QString("Trigger %1 set.").arg(triggerName));

    if (triggerName == "onspawn") {
        object->killAllTimers();
        object->invokeTrigger(triggerName);
        send(QString("Respawn emulated for %1.")
             .arg(Util::highlight(QString("object #%1").arg(object->id()))));
    }
}
