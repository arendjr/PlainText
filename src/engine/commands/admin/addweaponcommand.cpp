#include "addweaponcommand.h"

#include "util.h"
#include "weapon.h"


AddWeaponCommand::AddWeaponCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("Add a weapon to the current area.\n"
                   "\n"
                   "Usage: add-weapon <weapon-name>");
}

AddWeaponCommand::~AddWeaponCommand() {
}

void AddWeaponCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Usage: add-weapon <weapon-name>")) {
        return;
    }

    QString weaponName = takeWord();

    Weapon *weapon = GameObject::createByObjectType<Weapon *>(realm(), "weapon");
    weapon->setName(weaponName);
    currentArea()->addItem(weapon);

    send(QString("Weapon %1 added.").arg(weaponName));
}
