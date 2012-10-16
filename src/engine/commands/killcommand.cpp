#include "killcommand.h"

#include "util.h"


#define super Command

KillCommand::KillCommand(QObject *parent) :
    super(parent) {

    setDescription("Attacks another character.\n"
                   "\n"
                   "Examples: kill joe, attack earl");
}

KillCommand::~KillCommand() {
}

void KillCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr character = takeObject(currentRoom()->characters());
    if (!requireSome(character, "Kill who?")) {
        return;
    }

    player->kill(character);
}
