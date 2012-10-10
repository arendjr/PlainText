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

    if (!assertWordsLeft("Kill who?")) {
        return;
    }

    takeWord("the");

    QPair <QString, uint> description = takeObjectsDescription();
    GameObjectPtrList characters = objectsByDescription(description,
                                                        currentRoom()->characters());
    if (!requireSome(characters, "That character is not here.")) {
        return;
    }

    player->kill(characters[0]);
}
