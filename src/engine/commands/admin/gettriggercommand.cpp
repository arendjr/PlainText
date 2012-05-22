#include "gettriggercommand.h"


GetTriggerCommand::GetTriggerCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Show the script for some object's trigger.\n"
                   "\n"
                   "Usage: get-trigger <object-name> [#] <trigger-name>");
}

GetTriggerCommand::~GetTriggerCommand() {
}

void GetTriggerCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    GameObjectPtrList objects = takeObjects(currentArea()->objects());
    if (!requireUnique(objects, "Object not found.", "Object is not unique.")) {
        return;
    }

    QString triggerName = takeWord().toLower();

    if (!triggerName.startsWith("on")) {
        player()->send("Not a valid trigger name.");
        return;
    }

    ScriptFunction trigger = objects[0]->trigger(triggerName);
    if (trigger.source.isEmpty()) {
        player()->send(QString("No trigger set for %1.").arg(triggerName));
    } else {
        player()->send(trigger.source);
    }
}
