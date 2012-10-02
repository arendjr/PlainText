#ifndef BUYCOMMAND_H
#define BUYCOMMAND_H

#include "command.h"


class BuyCommand : public Command {

    Q_OBJECT

    public:
        BuyCommand(QObject *parent = 0);
        virtual ~BuyCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // BUYCOMMAND_H
