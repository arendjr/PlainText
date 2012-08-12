#ifndef LOSECOMMAND_H
#define LOSECOMMAND_H

#include "command.h"


class LoseCommand : public Command {

    Q_OBJECT

    public:
        LoseCommand(Player *player, QObject *parent = 0);
        virtual ~LoseCommand();

        virtual void execute(const QString &command);
};

#endif // LOSECOMMAND_H
