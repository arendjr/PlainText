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

    QString name = takeWord("the");

    QPair <QString, uint> description = takeObjectsDescription();
    GameObjectPtrList characters = objectsByDescription(description, currentArea()->characters());

    if (characters.isEmpty()) {
        if (name.isEmpty()) {
            name = Util::capitalize(description.first);
        } else {
            name = "The " + description.first;
        }
        player()->send(QString("%1 is not here.").arg(name));
        return;
    }

    QString message = takeRest();

    player()->talk(characters[0], message);
}
