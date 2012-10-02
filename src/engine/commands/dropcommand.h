#ifndef DROPCOMMAND_H
#define DROPCOMMAND_H

#include "command.h"


class DropCommand : public Command {

    Q_OBJECT

    public:
        DropCommand(QObject *parent = 0);
        virtual ~DropCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // DROPCOMMAND_H
