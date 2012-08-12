#ifndef DISBANDCOMMAND_H
#define DISBANDCOMMAND_H

#include "command.h"


class DisbandCommand : public Command {

    Q_OBJECT

    public:
        DisbandCommand(Player *player, QObject *parent = 0);
        virtual ~DisbandCommand();

        virtual void execute(const QString &command);
};

#endif // DISBANDCOMMAND_H
