#include "getcommand.h"

#include "util.h"


#define super Command

GetCommand::GetCommand(QObject *parent) :
    super(parent) {

    setDescription("Take an item or gold from the current area and put it in your inventory.\n"
                   "\n"
                   "Examples: get stick, take stick, get gold");
}

GetCommand::~GetCommand() {
}

void GetCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    if (!assertWordsLeft(QString("%1 what?").arg(Util::capitalize(alias())))) {
        return;
    }

    takeWord("the");

    GameObjectPtrList items = takeObjects(currentArea()->items());
    if (!requireSome(items, "That's not here.")) {
        return;
    }

    player->take(items);
}
