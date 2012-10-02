#include "unsettriggercommand.h"


#define super AdminCommand

UnsetTriggerCommand::UnsetTriggerCommand(QObject *parent) :
    super(parent) {

    setDescription("Removes the script for some object's trigger.\n"
                   "\n"
                   "Usage: unset-trigger <object-name> [#] <trigger-name>");
}

UnsetTriggerCommand::~UnsetTriggerCommand() {
}

void UnsetTriggerCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    GameObjectPtr object = takeObject(currentArea()->objects());
    if (!requireSome(object, "Object not found.")) {
        return;
    }

    QString triggerName = takeWord().toLower();

    if (!triggerName.startsWith("on")) {
        send("Not a valid trigger name.");
        return;
    }

    object->unsetTrigger(triggerName);
    send(QString("Trigger %1 unset.").arg(triggerName));
}
