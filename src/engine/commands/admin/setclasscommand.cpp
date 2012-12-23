#include "setclasscommand.h"

#include "character.h"
#include "realm.h"


#define super AdminCommand

SetClassCommand::SetClassCommand(QObject *parent) :
    super(parent) {

    setDescription("Set the class of some character.\n"
                   "\n"
                   "Usage: set-class <character-name> [#] <class-name>");
}

SetClassCommand::~SetClassCommand() {
}

void SetClassCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtrList characters = takeObjects(currentRoom()->characters());
    if (!requireUnique(characters, "Character not found.", "Character is not unique.")) {
        return;
    }

    QString className = takeWord();
    if (className.isEmpty()) {
        send("Usage: set-class <character-name> [#] <class-name>");
        return;
    }

    Character *character = characters[0].cast<Character *>();
    for (const GameObjectPtr &classPtr : Realm::instance()->classes()) {
        if (classPtr->name() == className) {
            character->setClass(classPtr);

            send("Class modified.");
            return;
        }
    }

    send("Unknown class given.");
}
