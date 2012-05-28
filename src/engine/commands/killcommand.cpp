#include "killcommand.h"

#include "engine/util.h"


KillCommand::KillCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Attacks another character."
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
    if (!requireUnique(characters,
                       QString("%1 is not here.").arg(Util::capitalize(description.first)),
                       QString("%1 is not unique.").arg(Util::capitalize(description.first)))) {
        return;
    }

    player()->kill(characters[0]);
}
