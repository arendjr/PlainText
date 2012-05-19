#include "talkcommand.h"

#include "engine/character.h"
#include "engine/item.h"
#include "engine/util.h"


TalkCommand::TalkCommand(Player *character, QObject *parent) :
    Command(character, parent) {
}

TalkCommand::~TalkCommand() {
}

void TalkCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Talk to who?")) {
        return;
    }

    QPair <QString, uint> description = takeObjectsDescription();
    GameObjectPtrList characters = objectsByDescription(description,
                                                        currentArea()->players() +
                                                        currentArea()->npcs());
    if (!requireUnique(characters,
                       QString("%1 is not here.").arg(description.first),
                       QString("%1 is not unique.").arg(description.first))) {
        return;
    }

    QString message = takeRest();

    player()->talk(characters[0], message);
}
