#ifndef TRIGGERSETCOMMAND_H
#define TRIGGERSETCOMMAND_H

#include "apicommand.h"


class TriggerSetCommand : public ApiCommand {

    public:
        TriggerSetCommand(QObject *parent = 0);
        virtual ~TriggerSetCommand();

        virtual void execute(Character *character, const QString &command);

    protected:
        virtual void setCommand(const QString &command);
};

#endif // TRIGGERSETCOMMAND_H
