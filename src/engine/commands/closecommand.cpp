#include "closecommand.h"


CloseCommand::CloseCommand(Player *player, QObject *parent) :
    Command(player, parent) {
}

CloseCommand::~CloseCommand() {
}

void CloseCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Close what?")) {
        return;
    }

    GameObjectPtrList exits = takeObjects(currentArea()->exits());
    if (!requireUnique(exits, "That's not here.", "Exit is not unique.")) {
        return;
    }

    player()->close(exits[0]);
}
