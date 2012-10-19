#include "tellcommand.h"

#include "realm.h"
#include "util.h"


#define super Command

TellCommand::TellCommand(QObject *parent) :
    super(parent) {

    setDescription("Tell something to another online player (no need to be in the same area). Only "
                   "this player will hear you.\n"
                   "\n"
                   "Example: tell earl Hey Earl, where are you?");
}

TellCommand::~TellCommand() {
}

void TellCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Tell who?")) {
        return;
    }

    QString userName = Util::capitalize(takeWord());
    Player *other = Realm::instance()->getPlayer(userName);
    if (!other || !other->session()) {
        send("%1 is not online.", userName);
        return;
    }

    QString message = takeRest();

    player->tell(other, message);
}
