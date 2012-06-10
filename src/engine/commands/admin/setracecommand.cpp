#include "setracecommand.h"

#include "engine/character.h"
#include "engine/realm.h"


SetRaceCommand::SetRaceCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Set the race of some character.\n"
                   "\n"
                   "Usage: set-race <character-name> [#] <race-name>");
}

SetRaceCommand::~SetRaceCommand() {
}

void SetRaceCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    GameObjectPtrList characters = takeObjects(currentArea()->characters());
    if (!requireUnique(characters, "Character not found.", "Character is not unique.")) {
        return;
    }

    Character *character = characters[0].cast<Character *>();

    QString raceName = takeWord();
    if (raceName.isEmpty()) {
        send("Usage: set-race <character-name> [#] <race-name>");
        return;
    }

    foreach (const GameObjectPtr &racePtr, Realm::instance()->races()) {
        if (racePtr->name() == raceName) {
            character->setRace(racePtr);

            send("Race modified.");
            return;
        }
    }
}
