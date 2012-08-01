#ifndef UNSETTRIGGERCOMMAND_H
#define UNSETTRIGGERCOMMAND_H

#include "admincommand.h"


class UnsetTriggerCommand : public AdminCommand {

    Q_OBJECT

    public:
        UnsetTriggerCommand(Player *player, QObject *parent = 0);
        virtual ~UnsetTriggerCommand();

        virtual void execute(const QString &command);
};

#endif // UNSETTRIGGERCOMMAND_H
