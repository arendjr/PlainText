#include "addcharactercommand.h"

#include "character.h"
#include "realm.h"


#define super AdminCommand

AddCharacterCommand::AddCharacterCommand(QObject *parent) :
    super(parent) {

    setDescription("Add a character to the current room.\n"
                   "\n"
                   "Usage: add-character <character-name>");
}

AddCharacterCommand::~AddCharacterCommand() {
}

void AddCharacterCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: add-character <character-name>")) {
        return;
    }

    QString characterName = takeWord();

    Character *character = new Character(realm());
    character->setName(characterName);
    character->setCurrentRoom(currentRoom());
    currentRoom()->addNPC(character);

    send("Character %1 added.", characterName);
}
