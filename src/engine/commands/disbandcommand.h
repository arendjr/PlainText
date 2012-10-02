#ifndef DISBANDCOMMAND_H
#define DISBANDCOMMAND_H

#include "command.h"


class DisbandCommand : public Command {

    Q_OBJECT

    public:
        DisbandCommand(QObject *parent = 0);
        virtual ~DisbandCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // DISBANDCOMMAND_H
