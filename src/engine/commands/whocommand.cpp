#include "whocommand.h"

#include "engine/realm.h"


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
        send("Only you are online.");
    } else {
        send(QString("%1 are online.").arg(players.joinFancy(Capitalized)));
    }
}
