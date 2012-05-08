#ifndef STOPSERVERCOMMAND_H
#define STOPSERVERCOMMAND_H

#include "admincommand.h"


class StopServerCommand : public AdminCommand {

    Q_OBJECT

    public:
        explicit StopServerCommand(Character *character, QObject *parent = 0);
        virtual ~StopServerCommand();

        virtual void execute(const QString &command);
};

#endif // STOPSERVERCOMMAND_H
