#include "gettriggercommand.h"


GetTriggerCommand::GetTriggerCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {
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

    player()->send(objects[0]->trigger(triggerName).source);
}
