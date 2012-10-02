#ifndef EQUIPMENTCOMMAND_H
#define EQUIPMENTCOMMAND_H

#include "command.h"


class EquipmentCommand : public Command {

    Q_OBJECT

    public:
        EquipmentCommand(QObject *parent = 0);
        virtual ~EquipmentCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // EQUIPMENTCOMMAND_H
