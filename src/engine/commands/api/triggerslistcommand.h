#ifndef TRIGGERSLISTCOMMAND_H
#define TRIGGERSLISTCOMMAND_H

#include "apicommand.h"


class TriggersListCommand : public ApiCommand {

    public:
        TriggersListCommand(Player *player, QObject *parent = 0);
        virtual ~TriggersListCommand();

        virtual void execute(const QString &command);
};

#endif // TRIGGERSLISTCOMMAND_H
