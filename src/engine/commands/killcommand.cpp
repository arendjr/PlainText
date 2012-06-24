#include "killcommand.h"

#include "engine/util.h"


KillCommand::KillCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Attacks another character.\n"
                   "\n"
                   "Examples: kill joe, attack earl");
}

KillCommand::~KillCommand() {
}

void KillCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Kill who?")) {
        return;
    }

    takeWord("the");

    QPair <QString, uint> description = takeObjectsDescription();
    GameObjectPtrList characters = objectsByDescription(description,
                                                        currentArea()->characters());
    if (!requireSome(characters, "That character is not here.")) {
        return;
    }

    player()->kill(characters[0]);
}
