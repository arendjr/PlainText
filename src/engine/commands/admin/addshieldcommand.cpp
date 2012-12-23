#include "addshieldcommand.h"

#include "shield.h"


#define super AdminCommand

AddShieldCommand::AddShieldCommand(QObject *parent) :
    super(parent) {

    setDescription("Add a shield to the current room.\n"
                   "\n"
                   "Usage: add-shield <shield-name>");
}

AddShieldCommand::~AddShieldCommand() {
}

void AddShieldCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: add-shield <shield-name>")) {
        return;
    }

    QString shieldName = takeWord();

    Shield *shield = new Shield(realm());
    shield->setName(shieldName);
    currentRoom()->addItem(shield);

    send("Shield %1 added.", shieldName);
}
