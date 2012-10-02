#ifndef SETTRIGGERCOMMAND_H
#define SETTRIGGERCOMMAND_H

#include "admincommand.h"


class SetTriggerCommand : public AdminCommand {

    Q_OBJECT

    public:
        SetTriggerCommand(QObject *parent = 0);
        virtual ~SetTriggerCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // SETTRIGGERCOMMAND_H
