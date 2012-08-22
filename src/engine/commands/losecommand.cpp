#include "losecommand.h"

#include "engine/util.h"


LoseCommand::LoseCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Remove yourself or someone else from a group. If you are a group leader, you "
                   "can remove anyone from your group by using *lose <name>*. You can always "
                   "remove yourself from a group using simply *lose*.\n"
                   "\n"
                   "Examples: lose mia, lose");
}

LoseCommand::~LoseCommand() {
}

void LoseCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    if (hasWordsLeft()) {
        QPair <QString, uint> description = takeObjectsDescription();
        GameObjectPtrList characters = objectsByDescription(description,
                                                            currentArea()->characters());

        if (characters.isEmpty()) {
            send(QString("\"%1\" is not here.").arg(Util::capitalize(description.first)));
        } else {
            player()->lose(characters[0]);
        }
    } else {
        player()->lose();
    }
}
