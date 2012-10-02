#include "losecommand.h"

#include "util.h"


#define super Command

LoseCommand::LoseCommand(QObject *parent) :
    super(parent) {

    setDescription("Remove yourself or someone else from a group. If you are a group leader, you "
                   "can remove anyone from your group by using *lose <name>*. You can always "
                   "remove yourself from a group using simply *lose*.\n"
                   "\n"
                   "Examples: lose mia, lose");
}

LoseCommand::~LoseCommand() {
}

void LoseCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    if (hasWordsLeft()) {
        QPair <QString, uint> description = takeObjectsDescription();
        GameObjectPtrList characters = objectsByDescription(description,
                                                            currentArea()->characters());

        if (characters.isEmpty()) {
            send(QString("\"%1\" is not here.").arg(Util::capitalize(description.first)));
        } else {
            player->lose(characters[0]);
        }
    } else {
        player->lose();
    }
}
