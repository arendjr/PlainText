#ifndef KILLCOMMAND_H
#define KILLCOMMAND_H

#include "command.h"


class KillCommand : public Command {

    Q_OBJECT

    public:
        KillCommand(QObject *parent = 0);
        virtual ~KillCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // KILLCOMMAND_H
