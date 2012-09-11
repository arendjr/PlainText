#include "copytriggerscommand.h"

#include "item.h"
#include "util.h"


CopyTriggersCommand::CopyTriggersCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Copies the triggers of one item to another.\n"
                   "\n"
                   "Usage: copy-triggers <source-item> [#] <destination-item> [#]");
}

CopyTriggersCommand::~CopyTriggersCommand() {
}

void CopyTriggersCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: copy-triggers <source-item> [#] <destination-item> [#]")) {
        return;
    }

    GameObjectPtr sourceItem = takeObject(currentArea()->items() + currentArea()->npcs());
    if (!requireSome(sourceItem, "Source item not found.")) {
        return;
    }

    GameObjectPtr destinationItem = takeObject(currentArea()->items() + currentArea()->npcs());
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
