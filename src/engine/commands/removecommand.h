#ifndef REMOVECOMMAND_H
#define REMOVECOMMAND_H

#include "command.h"


class RemoveCommand : public Command {

    Q_OBJECT

    public:
        RemoveCommand(Player *player, QObject *parent = 0);
        virtual ~RemoveCommand();

        virtual void execute(const QString &command);
};

#endif // REMOVECOMMAND_H
