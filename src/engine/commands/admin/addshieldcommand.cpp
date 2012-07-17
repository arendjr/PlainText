#include "addshieldcommand.h"

#include "engine/shield.h"
#include "engine/util.h"


AddShieldCommand::AddShieldCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Add a shield to the current area.\n"
                   "\n"
                   "Usage: add-shield <shield-name>");
}

AddShieldCommand::~AddShieldCommand() {
}

void AddShieldCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: add-shield <shield-name>")) {
        return;
    }

    QString shieldName = takeWord();

    Shield *shield = GameObject::createByObjectType<Shield *>(realm(), "shield");
    shield->setName(shieldName);
    currentArea()->addItem(shield);

    send(QString("Shield %1 added.").arg(shieldName));
}
