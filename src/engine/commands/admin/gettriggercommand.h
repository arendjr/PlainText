#ifndef GETTRIGGERCOMMAND_H
#define GETTRIGGERCOMMAND_H

#include "admincommand.h"


class GetTriggerCommand : public AdminCommand {

    Q_OBJECT

    public:
        GetTriggerCommand(QObject *parent = 0);
        virtual ~GetTriggerCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // GETTRIGGERCOMMAND_H
