#include "lookcommand.h"

#include "engine/item.h"
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
    if (!hasWordsLeft()) {
        player()->look();
        return;
    }

    takeWord("at", IfNotLast);
    takeWord("the");

    if (!assertWordsLeft("Look at what?")) {
        return;
    }

    GameObjectPtrList pool = player()->inventory() + currentArea()->objects();
    GameObjectPtrList objects = takeObjects(pool);
    if (!requireSome(objects, "That's not here.")) {
        return;
    }

    QString description = objects[0]->description();
    if (description.isEmpty()) {
        QString name;
        if (objects[0]->isExit()) {
            name = "the " + objects[0]->name();
        } else {
            name = objects[0].cast<Item *>()->definiteName(pool);
        }
        send(QString("There's nothing special about %1.").arg(name));
    } else {
        send(description);
    }
}
