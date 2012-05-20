#include "inventorycommand.h"

#include "engine/util.h"


InventoryCommand::InventoryCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Shows your current inventory.\n"
                   "\n"
                   "Example: inventory");
}

InventoryCommand::~InventoryCommand() {
}

void InventoryCommand::execute(const QString &command) {

    setCommand(command);

    player()->send(QString("You carry %1.\n"
                           "You've got $%2 worth of gold.")
                   .arg(Util::joinItems(player()->inventory()),
                        player()->gold()));
}
