#ifndef TRIGGERSETCOMMAND_H
#define TRIGGERSETCOMMAND_H

#include "apicommand.h"


class TriggerSetCommand : public ApiCommand {

    public:
        TriggerSetCommand(Player *player, QObject *parent = 0);
        virtual ~TriggerSetCommand();

        virtual void execute(const QString &command);
};

#endif // TRIGGERSETCOMMAND_H
