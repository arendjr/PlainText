#ifndef ADDITEMCOMMAND_H
#define ADDITEMCOMMAND_H

#include "admincommand.h"


class AddItemCommand : public AdminCommand {

    Q_OBJECT

    public:
        AddItemCommand(QObject *parent = 0);
        virtual ~AddItemCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // ADDITEMCOMMAND_H
