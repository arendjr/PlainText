#ifndef BUYCOMMAND_H
#define BUYCOMMAND_H

#include "command.h"


class BuyCommand : public Command {

    Q_OBJECT

    public:
        explicit BuyCommand(Player *player, QObject *parent = 0);
        virtual ~BuyCommand();

        virtual void execute(const QString &command);
};

#endif // BUYCOMMAND_H
