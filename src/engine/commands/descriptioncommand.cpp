#include "descriptioncommand.h"

#include "util.h"


#define super Command

DescriptionCommand::DescriptionCommand(QObject *parent) :
    super(parent) {

    setDescription("Change the description of your own character (max. 140 characters). Others "
                   "will see this description when they look at you.\n"
                   "\n"
                   "Example: description Wearing his axe on his back, he is a rather fearsome "
                                        "warrior.");
}

DescriptionCommand::~DescriptionCommand() {
}

void DescriptionCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Set your description to what?")) {
        return;
    }

    QString description = Util::capitalize(takeRest().left(140));
    if (!description.endsWith(".") && !description.endsWith("!") && !description.endsWith("?")) {
        description.append(".");
    }

    player->setDescription(description);

    send("Your description has been changed to:\n" + description);
}
