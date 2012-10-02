#ifndef EATCOMMAND_H
#define EATCOMMAND_H

#include "command.h"


class EatCommand : public Command {

    Q_OBJECT

    public:
        EatCommand(QObject *parent = 0);
        virtual ~EatCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // EATCOMMAND_H
