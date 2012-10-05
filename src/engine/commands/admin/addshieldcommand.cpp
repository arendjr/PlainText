#include "addshieldcommand.h"

#include "shield.h"


#define super AdminCommand

AddShieldCommand::AddShieldCommand(QObject *parent) :
    super(parent) {

    setDescription("Add a shield to the current area.\n"
                   "\n"
                   "Usage: add-shield <shield-name>");
}

AddShieldCommand::~AddShieldCommand() {
}

void AddShieldCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: add-shield <shield-name>")) {
        return;
    }

    QString shieldName = takeWord();

    Shield *shield = GameObject::createByObjectType<Shield *>(realm(), "shield");
    shield->setName(shieldName);
    currentArea()->addItem(shield);

    send(QString("Shield %1 added.").arg(shieldName));
}
