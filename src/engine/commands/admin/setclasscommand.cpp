#include "setclasscommand.h"

#include "engine/character.h"
#include "engine/race.h"


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

    Character *character = characters[0].cast<Character *>();
    if (character->race().isNull()) {
        send("Need to set the race first.");
        return;
    }

    QString className = takeWord();
    if (className.isEmpty()) {
        send("Usage: set-class <character-name> [#] <class-name>");
        return;
    }

    Race *race = character->race().cast<Race *>();
    foreach (const GameObjectPtr &classPtr, race->classes()) {
        if (classPtr->name() == className) {
            character->setClass(classPtr);

            send("Class modified.");
            return;
        }
    }
}
