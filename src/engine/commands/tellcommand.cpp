#include "tellcommand.h"

#include "realm.h"
#include "util.h"


TellCommand::TellCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Tell something to another online player (no need to be in "
                   "the same area). Only this player will hear you.\n"
                   "\n"
                   "Example: tell earl Hey Earl, where are you?");
}

TellCommand::~TellCommand() {
}

void TellCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Tell who?")) {
        return;
    }

    QString userName = Util::capitalize(takeWord());
    Player *other = Realm::instance()->getPlayer(userName);
    if (!other || !other->session()) {
        player()->send(QString("%1 is not online.").arg(userName));
        return;
    }

    QString message = takeRest();

    player()->tell(other, message);
}
