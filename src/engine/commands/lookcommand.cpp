#include "lookcommand.h"

#include <cstring>

#include "engine/util.h"


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

    takeWord("at", IfNotLast);
    takeWord("the");

    if (!assertWordsLeft("Look at what?")) {
        return;
    }

    GameObjectPtrList pool = player()->inventory() + currentArea()->exits() +
                             currentArea()->characters() + currentArea()->items();
    GameObjectPtrList objects = takeObjects(pool);
    if (!requireSome(objects, "That's not here.")) {
        return;
    }

    QString description = objects[0]->description();
    if (description.isEmpty()) {
        QString name;
        if (strcmp(objects[0]->objectType(), "exit") == 0) {
            name = "the " + objects[0]->name();
        } else {
            name = Util::definiteName(objects[0], pool);
        }
        player()->send(QString("There's nothing special about %1.").arg(name));
    } else {
        player()->send(description);
    }
}
