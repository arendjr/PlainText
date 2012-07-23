#ifndef DRINKCOMMAND_H
#define DRINKCOMMAND_H

#include "command.h"


class DrinkCommand : public Command {

    Q_OBJECT

    public:
        DrinkCommand(Player *player, QObject *parent = 0);
        virtual ~DrinkCommand();

        virtual void execute(const QString &command);
};

#endif // DRINKCOMMAND_H
