#ifndef REMOVEITEMCOMMAND_H
#define REMOVEITEMCOMMAND_H

#include "admincommand.h"


class RemoveItemCommand : public AdminCommand {

    Q_OBJECT

    public:
        explicit RemoveItemCommand(Character *character, QObject *parent = 0);
        virtual ~RemoveItemCommand();

        virtual void execute(const QString &command);
};

#endif // REMOVEITEMCOMMAND_H
