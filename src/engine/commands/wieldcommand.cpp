#include "wieldcommand.h"

#include "engine/util.h"


WieldCommand::WieldCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Wield a weapon or shield that's in your inventory.\n"
                   "\n"
                   "Example: wield sword");
}

WieldCommand::~WieldCommand() {
}

void WieldCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Wield what?")) {
        return;
    }

    takeWord("(the|my)");

    GameObjectPtrList allItems = player()->inventory();
    GameObjectPtr item = takeObject(allItems);
    if (!requireSome(item, "You don't have that.")) {
        return;
    }

    player()->wield(item);
}
