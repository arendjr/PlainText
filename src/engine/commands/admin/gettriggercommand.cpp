#include "gettriggercommand.h"

#include "util.h"


#define super AdminCommand

GetTriggerCommand::GetTriggerCommand(QObject *parent) :
    super(parent) {

    setDescription("Show the script for some object's trigger.\n"
                   "\n"
                   "Usage: get-trigger <object-name> [#] <trigger-name>\n"
                   "\n"
                   "Type *help triggers* to see a list of all available triggers.");
}

GetTriggerCommand::~GetTriggerCommand() {
}

void GetTriggerCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = takeObject(currentRoom()->exits() + currentRoom()->characters() +
                                      currentRoom()->items());
    if (!requireSome(object, "Object not found.")) {
        return;
    }

    QString triggerName = takeWord().toLower();

    if (!triggerName.startsWith("on")) {
        send("Not a valid trigger name.");
        return;
    }

    ScriptFunction trigger = object->trigger(triggerName);
    if (trigger.source.isEmpty()) {
        send(QString("No trigger set for %1.").arg(triggerName));
    } else {
        send(trigger.source);
    }
}
