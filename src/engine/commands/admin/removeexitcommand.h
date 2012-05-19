#ifndef REMOVEEXITCOMMAND_H
#define REMOVEEXITCOMMAND_H

#include "admincommand.h"


class RemoveExitCommand : public AdminCommand {

    Q_OBJECT

    public:
        explicit RemoveExitCommand(Player *player, QObject *parent = 0);
        virtual ~RemoveExitCommand();

        virtual void execute(const QString &command);
};

#endif // REMOVEEXITCOMMAND_H
