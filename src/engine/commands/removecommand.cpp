#include "removecommand.h"

#include "util.h"


RemoveCommand::RemoveCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Remove an item that is currently worn or wielded and put it back in your "
                   "inventory.\n"
                   "\n"
                   "Example: remove item");
}

RemoveCommand::~RemoveCommand() {
}

void RemoveCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Remove what?")) {
        return;
    }

    takeWord("(the|my)");

    GameObjectPtrList allItems;
    allItems << player()->weapon()
             << player()->secondaryWeapon()
             << player()->shield();

    GameObjectPtr item = takeObject(allItems);
    if (!requireSome(item, "You're not wearing or wielding that.")) {
        return;
    }

    player()->remove(item);
}
