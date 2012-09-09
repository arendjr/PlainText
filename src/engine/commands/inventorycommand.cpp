#include "inventorycommand.h"


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

    QString carriedInventoryString;
    if (player()->inventory().isEmpty()) {
        carriedInventoryString = "You don't carry anything.\n";
    } else if (player()->inventory().length() == 1) {
        carriedInventoryString = QString("You carry %1, weighing %2kg.\n")
                                 .arg(player()->inventory().joinFancy(),
                                      QString::number(player()->inventoryWeight()));
    } else {
        carriedInventoryString = QString("You carry %1, weighing a total of %2kg.\n")
                                 .arg(player()->inventory().joinFancy(),
                                      QString::number(player()->inventoryWeight()));
    }

    QString carriedGoldString;
    if (player()->gold() == 0.0) {
        carriedGoldString = "You don't have any gold.\n";
    } else {
        carriedGoldString = QString("You've got $%1 worth of gold.\n").arg(player()->gold());
    }

    send(carriedInventoryString + carriedGoldString);
}
