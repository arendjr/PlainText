#ifndef TRIGGERGETCOMMAND_H
#define TRIGGERGETCOMMAND_H

#include "apicommand.h"


class TriggerGetCommand : public ApiCommand {

    public:
        TriggerGetCommand(QObject *parent = 0);
        virtual ~TriggerGetCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // TRIGGERGETCOMMAND_H
