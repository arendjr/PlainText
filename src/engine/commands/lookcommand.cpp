#include "lookcommand.h"


LookCommand::LookCommand(Player *character, QObject *parent) :
    Command(character, parent) {
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
        player()->send(QString("There's nothing special about the %1.").arg(name));
    } else {
        player()->send(description);
    }
}
