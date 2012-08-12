#ifndef GROUPCOMMAND_H
#define GROUPCOMMAND_H

#include "command.h"


class GroupCommand : public Command {

    Q_OBJECT

    public:
        GroupCommand(Player *player, QObject *parent = 0);
        virtual ~GroupCommand();

        virtual void execute(const QString &command);
};

#endif // GROUPCOMMAND_H
