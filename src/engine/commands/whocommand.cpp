#include "whocommand.h"

#include "engine/realm.h"
#include "engine/util.h"


WhoCommand::WhoCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Show who is currently online.\n"
                   "\n"
                   "Example: who");
}

WhoCommand::~WhoCommand() {
}

void WhoCommand::execute(const QString &command) {

    setCommand(command);

    GameObjectPtrList players = Realm::instance()->onlinePlayers();
    if (players.length() == 1) {
        player()->send("Only you are online.");
    } else {
        player()->send(QString("%1 are online.").arg(Util::joinItems(players)));
    }
}
