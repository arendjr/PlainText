#ifndef ADDSHIELDCOMMAND_H
#define ADDSHIELDCOMMAND_H

#include "admincommand.h"


class AddShieldCommand : public AdminCommand {

    Q_OBJECT

    public:
        AddShieldCommand(QObject *parent = 0);
        virtual ~AddShieldCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // ADDSHIELDCOMMAND_H
