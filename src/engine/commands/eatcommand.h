#ifndef EATCOMMAND_H
#define EATCOMMAND_H

#include "command.h"


class EatCommand : public Command {

    Q_OBJECT

    public:
        explicit EatCommand(Player *player, QObject *parent = 0);
        virtual ~EatCommand();

        virtual void execute(const QString &command);
};

#endif // EATCOMMAND_H
