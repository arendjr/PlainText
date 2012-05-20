#include "lookcommand.h"


LookCommand::LookCommand(Player *character, QObject *parent) :
    Command(character, parent) {

    setDescription("Look at something (any object, character, or the current "
                   "area).\n"
                   "\n"
                   "Examples: look, look door, look earl");
}

LookCommand::~LookCommand() {
}

void LookCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (numWordsLeft() == 0) {
        player()->look();
        return;
    }

    takeWord("at");
    takeWord("the");

    if (!assertWordsLeft("Look at what?")) {
        return;
    }

    GameObjectPtrList objects = takeObjects(player()->inventory() +
                                            currentArea()->exits() +
                                            currentArea()->characters() +
                                            currentArea()->items());
    if (!requireUnique(objects, "That's not here.", "Not unique.")) {
        return;
    }

    QString description = objects[0]->description();
    if (description.isEmpty()) {
        QString name = objects[0]->name();
        if (name[0].toLower() != name[0]) {
            player()->send(QString("There's nothing special about %1.").arg(name));
        } else {
            player()->send(QString("There's nothing special about the %1.").arg(name));
        }
    } else {
        player()->send(description);
    }
}
