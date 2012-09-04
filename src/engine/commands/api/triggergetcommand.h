#ifndef TRIGGERGETCOMMAND_H
#define TRIGGERGETCOMMAND_H

#include "apicommand.h"


class TriggerGetCommand : public ApiCommand {

    public:
        TriggerGetCommand(Player *player, QObject *parent = 0);
        virtual ~TriggerGetCommand();

        virtual void execute(const QString &command);
};

#endif // TRIGGERGETCOMMAND_H
