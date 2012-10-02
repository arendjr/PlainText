#include "wieldcommand.h"

#include "util.h"


#define super Command

WieldCommand::WieldCommand(QObject *parent) :
    super(parent) {

    setDescription("Wield a weapon or shield that's in your inventory.\n"
                   "\n"
                   "Example: wield sword");
}

WieldCommand::~WieldCommand() {
}

void WieldCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    if (!assertWordsLeft("Wield what?")) {
        return;
    }

    takeWord("(the|my)");

    GameObjectPtrList allItems = player->inventory();
    GameObjectPtr item = takeObject(allItems);
    if (!requireSome(item, "You don't have that.")) {
        return;
    }

    player->wield(item);
}
