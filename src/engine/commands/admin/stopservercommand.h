#ifndef STOPSERVERCOMMAND_H
#define STOPSERVERCOMMAND_H

#include "admincommand.h"


class StopServerCommand : public AdminCommand {

    Q_OBJECT

    public:
        StopServerCommand(QObject *parent = 0);
        virtual ~StopServerCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // STOPSERVERCOMMAND_H
