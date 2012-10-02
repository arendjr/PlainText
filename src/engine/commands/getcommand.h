#ifndef GETCOMMAND_H
#define GETCOMMAND_H

#include "command.h"


class GetCommand : public Command {

    Q_OBJECT

    public:
        GetCommand(QObject *parent = 0);
        virtual ~GetCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // GETCOMMAND_H
