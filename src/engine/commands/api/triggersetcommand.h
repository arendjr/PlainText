#ifndef TRIGGERSETCOMMAND_H
#define TRIGGERSETCOMMAND_H

#include "apicommand.h"


class TriggerSetCommand : public ApiCommand {

    public:
        TriggerSetCommand(QObject *parent = 0);
        virtual ~TriggerSetCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // TRIGGERSETCOMMAND_H
