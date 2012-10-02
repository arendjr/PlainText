#include "addweaponcommand.h"

#include "weapon.h"


#define super AdminCommand

AddWeaponCommand::AddWeaponCommand(QObject *parent) :
    super(parent) {

    setDescription("Add a weapon to the current area.\n"
                   "\n"
                   "Usage: add-weapon <weapon-name>");
}

AddWeaponCommand::~AddWeaponCommand() {
}

void AddWeaponCommand::execute(Player *player, const QString &command) {

    super::execute(player, command);

    if (!assertWordsLeft("Usage: add-weapon <weapon-name>")) {
        return;
    }

    QString weaponName = takeWord();

    Weapon *weapon = GameObject::createByObjectType<Weapon *>(realm(), "weapon");
    weapon->setName(weaponName);
    currentArea()->addItem(weapon);

    send(QString("Weapon %1 added.").arg(weaponName));
}
