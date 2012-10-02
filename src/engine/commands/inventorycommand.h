#ifndef INVENTORYCOMMAND_H
#define INVENTORYCOMMAND_H

#include "command.h"


class InventoryCommand : public Command {

    Q_OBJECT

    public:
        InventoryCommand(QObject *parent = 0);
        virtual ~InventoryCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // INVENTORYCOMMAND_H
