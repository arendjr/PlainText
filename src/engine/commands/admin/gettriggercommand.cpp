#include "gettriggercommand.h"

#include "engine/util.h"


GetTriggerCommand::GetTriggerCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription(QString("Show the script for some object's trigger.\n"
                           "\n"
                           "Usage: get-trigger <object-name> [#] <trigger-name>\n"
                           "\n"
                           "Type %1 to see a list of all available triggers.")
                   .arg(Util::highlight("help triggers")));
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
