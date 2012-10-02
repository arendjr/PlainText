#include "gtalkcommand.h"

#include "util.h"


#define super Command

GtalkCommand::GtalkCommand(QObject *parent) :
    super(parent) {

    setDescription("Talk to everyone who's in the same group.\n"
                   "\n"
                   "Example: gtalk Where do we go now?");
}

GtalkCommand::~GtalkCommand() {
}

void GtalkCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    if (!assertWordsLeft("Say what?")) {
        return;
    }

    if (player->group().isNull()) {
        send("You are not in a group.");
    } else {
        QString message = takeRest();
        QString text = (message.endsWith(".") || message.endsWith("?") || message.endsWith("!")) ?
                       "(Group) %1 mentioned, \"%2\"" : "(Group) %1 mentioned, \"%2.\"";

        player->group()->send(text.arg(player->name(), message));
    }
}
