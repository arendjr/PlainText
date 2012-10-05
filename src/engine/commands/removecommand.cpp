#include "removecommand.h"

#include "util.h"


#define super Command

RemoveCommand::RemoveCommand(QObject *parent) :
    super(parent) {

    setDescription("Remove an item that is currently worn or wielded and put it back in your "
                   "inventory.\n"
                   "\n"
                   "Example: remove item");
}

RemoveCommand::~RemoveCommand() {
}

void RemoveCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Remove what?")) {
        return;
    }

    takeWord("(the|my)");

    GameObjectPtrList allItems;
    allItems << player->weapon()
             << player->secondaryWeapon()
             << player->shield();

    GameObjectPtr item = takeObject(allItems);
    if (!requireSome(item, "You're not wearing or wielding that.")) {
        return;
    }

    player->remove(item);
}
