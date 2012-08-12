#include "losecommand.h"

#include "engine/util.h"


LoseCommand::LoseCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription(QString("Remove yourself or someone else from a group. If you are a group "
                           "leader, you can remove anyone from your group by using %1. You can "
                           "always remove yourself from a group using simply %2.\n"
                           "\n"
                           "Examples: lose Mia, lose")
                   .arg(Util::highlight("lose <name>"), Util::highlight("lose")));
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
