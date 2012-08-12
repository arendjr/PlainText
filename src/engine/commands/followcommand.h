#ifndef FOLLOWCOMMAND_H
#define FOLLOWCOMMAND_H

#include "command.h"


class FollowCommand : public Command {

    Q_OBJECT

    public:
        FollowCommand(Player *player, QObject *parent = 0);
        virtual ~FollowCommand();

        virtual void execute(const QString &command);
};

#endif // FOLLOWCOMMAND_H
