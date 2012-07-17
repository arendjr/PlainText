#ifndef ADDWEAPONCOMMAND_H
#define ADDWEAPONCOMMAND_H

#include "admincommand.h"


class AddWeaponCommand : public AdminCommand {

    Q_OBJECT

    public:
        AddWeaponCommand(Player *player, QObject *parent = 0);
        virtual ~AddWeaponCommand();

        virtual void execute(const QString &command);
};

#endif // ADDWEAPONCOMMAND_H
