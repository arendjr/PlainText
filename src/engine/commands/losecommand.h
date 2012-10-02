#ifndef LOSECOMMAND_H
#define LOSECOMMAND_H

#include "command.h"


class LoseCommand : public Command {

    Q_OBJECT

    public:
        LoseCommand(QObject *parent = 0);
        virtual ~LoseCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // LOSECOMMAND_H
