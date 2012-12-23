#include "addweaponcommand.h"

#include "weapon.h"


#define super AdminCommand

AddWeaponCommand::AddWeaponCommand(QObject *parent) :
    super(parent) {

    setDescription("Add a weapon to the current room.\n"
                   "\n"
                   "Usage: add-weapon <weapon-name>");
}

AddWeaponCommand::~AddWeaponCommand() {
}

void AddWeaponCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    if (!assertWordsLeft("Usage: add-weapon <weapon-name>")) {
        return;
    }

    QString weaponName = takeWord();

    Weapon *weapon = new Weapon(realm());
    weapon->setName(weaponName);
    currentRoom()->addItem(weapon);

    send("Weapon %1 added.", weaponName);
}
