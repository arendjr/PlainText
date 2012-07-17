#ifndef ADDITEMCOMMAND_H
#define ADDITEMCOMMAND_H

#include "admincommand.h"


class AddItemCommand : public AdminCommand {

    Q_OBJECT

    public:
        AddItemCommand(Player *player, QObject *parent = 0);
        virtual ~AddItemCommand();

        virtual void execute(const QString &command);
};

#endif // ADDITEMCOMMAND_H
