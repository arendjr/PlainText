#ifndef KILLCOMMAND_H
#define KILLCOMMAND_H

#include "command.h"


class KillCommand : public Command {

    Q_OBJECT

    public:
        KillCommand(Player *player, QObject *parent = 0);
        virtual ~KillCommand();

        virtual void execute(const QString &command);
};

#endif // KILLCOMMAND_H
