#include "addcharactercommand.h"

#include "character.h"
#include "realm.h"
#include "util.h"


AddCharacterCommand::AddCharacterCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Add a character to the current area.\n"
                   "\n"
                   "Usage: add-character <character-name>");
}

AddCharacterCommand::~AddCharacterCommand() {
}

void AddCharacterCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: add-character <character-name>")) {
        return;
    }

    QString characterName = takeWord();

    Character *character = GameObject::createByObjectType<Character *>(realm(), "character");
    character->setName(characterName);
    character->setCurrentArea(currentArea());
    currentArea()->addNPC(character);

    player()->send(QString("Character %1 added.").arg(characterName));
}
