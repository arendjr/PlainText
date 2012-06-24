#include "setclasscommand.h"

#include "engine/character.h"
#include "engine/realm.h"


SetClassCommand::SetClassCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Set the class of some character.\n"
                   "\n"
                   "Usage: set-class <character-name> [#] <class-name>");
}

SetClassCommand::~SetClassCommand() {
}

void SetClassCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    GameObjectPtrList characters = takeObjects(currentArea()->characters());
    if (!requireUnique(characters, "Character not found.", "Character is not unique.")) {
        return;
    }

    QString className = takeWord();
    if (className.isEmpty()) {
        send("Usage: set-class <character-name> [#] <class-name>");
        return;
    }

    Character *character = characters[0].cast<Character *>();
    gopl_foreach (classPtr, Realm::instance()->classes()) {
        if (classPtr->name() == className) {
            character->setClass(classPtr);

            send("Class modified.");
            return;
        }
    }

    send("Unknown class given.");
}
