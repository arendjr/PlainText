#ifndef GOCOMMAND_H
#define GOCOMMAND_H

#include "command.h"


class GoCommand : public Command {

    Q_OBJECT

    public:
        GoCommand(Player *player, QObject *parent = 0);
        virtual ~GoCommand();

        virtual void execute(const QString &command);
};

#endif // GOCOMMAND_H
