#include "setracecommand.h"

#include "character.h"
#include "realm.h"


#define super AdminCommand

SetRaceCommand::SetRaceCommand(QObject *parent) :
    super(parent) {

    setDescription("Set the race of some character.\n"
                   "\n"
                   "Usage: set-race <character-name> [#] <race-name>");
}

SetRaceCommand::~SetRaceCommand() {
}

void SetRaceCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtrList characters = takeObjects(currentRoom()->characters());
    if (!requireUnique(characters, "Character not found.", "Character is not unique.")) {
        return;
    }

    QString raceName = takeWord();
    if (raceName.isEmpty()) {
        send("Usage: set-race <character-name> [#] <race-name>");
        return;
    }

    Character *character = characters[0].cast<Character *>();
    for (const GameObjectPtr &racePtr : Realm::instance()->races()) {
        if (racePtr->name() == raceName) {
            character->setRace(racePtr);

            send("Race modified.");
            return;
        }
    }

    send("Unknown race given.");
}
