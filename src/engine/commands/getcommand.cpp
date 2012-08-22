#include "getcommand.h"

#include "engine/util.h"


GetCommand::GetCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Take an item or gold from the current area and put it in your inventory.\n"
                   "\n"
                   "Examples: get stick, take stick, get gold");
}

GetCommand::~GetCommand() {
}

void GetCommand::execute(const QString &command) {

    setCommand(command);

    QString alias = takeWord();
    if (!assertWordsLeft(QString("%1 what?").arg(Util::capitalize(alias)))) {
        return;
    }

    takeWord("the");

    GameObjectPtrList items = takeObjects(currentArea()->items());
    if (!requireSome(items, "That's not here.")) {
        return;
    }

    player()->take(items);
}
