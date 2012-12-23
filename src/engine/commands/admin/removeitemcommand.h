#ifndef REMOVEITEMCOMMAND_H
#define REMOVEITEMCOMMAND_H

#include "admincommand.h"


class RemoveItemCommand : public AdminCommand {

    Q_OBJECT

    public:
        RemoveItemCommand(QObject *parent = 0);
        virtual ~RemoveItemCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // REMOVEITEMCOMMAND_H
