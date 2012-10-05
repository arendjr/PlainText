#include "talkcommand.h"

#include "util.h"


#define super Command

TalkCommand::TalkCommand(QObject *parent) :
    super(parent) {

    setDescription("Talk to a specific character who's in the same area. Only "
                   "this character will hear you.\n"
                   "\n"
                   "Example: talk earl Hey Earl, how are you?");
}

TalkCommand::~TalkCommand() {
}

void TalkCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

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
        send(QString("\"%1\" is not here.").arg(name));
        return;
    }

    if (!assertWordsLeft("Say what?")) {
        return;
    }

    QString message = takeRest();

    player->talk(characters[0], message);
}
