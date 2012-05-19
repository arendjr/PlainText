#include "opencommand.h"


OpenCommand::OpenCommand(Player *player, QObject *parent) :
    Command(player, parent) {
}

OpenCommand::~OpenCommand() {
}

void OpenCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Open what?")) {
        return;
    }

    GameObjectPtrList exits = takeObjects(currentArea()->exits());
    if (!requireUnique(exits, "That's not here.", "Exit is not unique.")) {
        return;
    }

    player()->open(exits[0]);
}
