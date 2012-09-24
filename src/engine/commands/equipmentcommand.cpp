#include "equipmentcommand.h"


EquipmentCommand::EquipmentCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("View any equipment you are currently wielding.\n"
                   "\n"
                   "Example: equipment");
}

EquipmentCommand::~EquipmentCommand() {
}

void EquipmentCommand::execute(const QString &command) {

    setCommand(command);

    GameObjectPtrList wieldedEquipment;
    if (!player()->weapon().isNull()) {
        wieldedEquipment << player()->weapon();
    }
    if (!player()->secondaryWeapon().isNull()) {
        wieldedEquipment << player()->secondaryWeapon();
    }
    if (!player()->shield().isNull()) {
        wieldedEquipment << player()->shield();
    }

    if (wieldedEquipment.isEmpty()) {
        send("You're not wielding anything.");
    } else {
        send(QString("You're wielding %1.").arg(wieldedEquipment.joinFancy()));
    }
}
