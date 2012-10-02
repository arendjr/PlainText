#ifndef REMOVECOMMAND_H
#define REMOVECOMMAND_H

#include "command.h"


class RemoveCommand : public Command {

    Q_OBJECT

    public:
        RemoveCommand(QObject *parent = 0);
        virtual ~RemoveCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // REMOVECOMMAND_H
