#ifndef REMOVEEXITCOMMAND_H
#define REMOVEEXITCOMMAND_H

#include "admincommand.h"


class RemoveExitCommand : public AdminCommand {

    Q_OBJECT

    public:
        RemoveExitCommand(QObject *parent = 0);
        virtual ~RemoveExitCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // REMOVEEXITCOMMAND_H
