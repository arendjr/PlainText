#include "gocommand.h"


GoCommand::GoCommand(Player *player, QObject *parent) :
    Command(player, parent) {
}

GoCommand::~GoCommand() {
}

void GoCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Go where?")) {
        return;
    }

    GameObjectPtrList exits = takeObjects(currentArea()->exits());
    if (!requireUnique(exits, "You can't go that way.", "Exit is not unique.")) {
        return;
    }

    player()->go(exits[0]);
}
