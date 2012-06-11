#include "gocommand.h"


GoCommand::GoCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Go to or enter an exit.\n"
                   "\n"
                   "Examples: go north, go to tower, enter door");
}

GoCommand::~GoCommand() {
}

void GoCommand::execute(const QString &command) {

    setCommand(command);

    QString alias = takeWord();
    if (alias == "go") {
        takeWord("to", IfNotLast);
    }
    takeWord("the");

    if (!assertWordsLeft("Go where?")) {
        return;
    }

    GameObjectPtrList exits = takeObjects(currentArea()->exits());
    if (!requireSome(exits, "You can't go that way.")) {
        return;
    }

    player()->go(exits[0]);
}
