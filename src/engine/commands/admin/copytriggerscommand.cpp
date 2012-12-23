#include "copytriggerscommand.h"

#include "item.h"
#include "util.h"


#define super AdminCommand

CopyTriggersCommand::CopyTriggersCommand(QObject *parent) :
    super(parent) {

    setDescription("Copies the triggers of one item to another.\n"
                   "\n"
                   "Usage: copy-triggers <source-item> [#] <destination-item> [#]");
}

CopyTriggersCommand::~CopyTriggersCommand() {
}

void CopyTriggersCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: copy-triggers <source-item> [#] <destination-item> [#]")) {
        return;
    }

    GameObjectPtr sourceItem = takeObject(currentRoom()->items() + currentRoom()->characters());
    if (!requireSome(sourceItem, "Source item not found.")) {
        return;
    }

    GameObjectPtr destinationItem = takeObject(currentRoom()->items() +
                                               currentRoom()->characters());
    if (!requireSome(destinationItem, "Destination item not found.")) {
        return;
    }

    destinationItem->setTriggers(sourceItem->triggers());
    send("Triggers copied.");

    if (destinationItem->hasTrigger("onspawn")) {
        destinationItem->killAllTimers();
        destinationItem->invokeTrigger("onspawn");
        send(QString("Respawn emulated for %1.")
             .arg(Util::highlight(QString("object #%1").arg(destinationItem->id()))));
    }
}
