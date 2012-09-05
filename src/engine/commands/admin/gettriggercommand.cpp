#include "gettriggercommand.h"

#include "util.h"


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

    GameObjectPtr object = takeObject(currentArea()->objects());
    if (!requireSome(object, "Object not found.")) {
        return;
    }

    QString triggerName = takeWord().toLower();

    if (!triggerName.startsWith("on")) {
        player()->send("Not a valid trigger name.");
        return;
    }

    ScriptFunction trigger = object->trigger(triggerName);
    if (trigger.source.isEmpty()) {
        player()->send(QString("No trigger set for %1.").arg(triggerName));
    } else {
        player()->send(trigger.source);
    }
}
