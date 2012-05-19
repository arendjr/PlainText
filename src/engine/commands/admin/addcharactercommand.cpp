#include "addcharactercommand.h"

#include "engine/character.h"
#include "engine/realm.h"
#include "engine/util.h"


AddCharacterCommand::AddCharacterCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {
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

    Character *character = qobject_cast<Character *>(GameObject::createByObjectType("character"));
    character->setName(characterName);
    character->setCurrentArea(currentArea());
    currentArea()->addNPC(character);

    player()->send(QString("Character %1 added.").arg(characterName));
}
