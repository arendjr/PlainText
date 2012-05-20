#include "unsettriggercommand.h"


UnsetTriggerCommand::UnsetTriggerCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Removes the script for some object's trigger.\n"
                   "\n"
                   "Usage: unset-trigger <object-name> [#] <trigger-name>");
}

UnsetTriggerCommand::~UnsetTriggerCommand() {
}

void UnsetTriggerCommand::execute(const QString &command) {

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

    objects[0]->unsetTrigger(triggerName);
    player()->send(QString("Trigger %1 unset.").arg(triggerName));
}
