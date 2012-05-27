#include "talkcommand.h"

#include "engine/util.h"


TalkCommand::TalkCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Talk to a specific character who's in the same area. Only "
                   "this character will hear you.\n"
                   "\n"
                   "Example: talk earl Hey Earl, how are you?");
}

TalkCommand::~TalkCommand() {
}

void TalkCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Talk to who?")) {
        return;
    }

    takeWord("to");

    QPair <QString, uint> description = takeObjectsDescription();
    GameObjectPtrList characters = objectsByDescription(description,
                                                        currentArea()->characters());
    if (!requireUnique(characters,
                       QString("%1 is not here.").arg(description.first),
                       QString("%1 is not unique.").arg(description.first))) {
        return;
    }

    QString message = takeRest();

    player()->talk(characters[0], message);
}
