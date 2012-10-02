#ifndef GETPROPCOMMAND_H
#define GETPROPCOMMAND_H

#include "admincommand.h"


class GetPropCommand : public AdminCommand {

    Q_OBJECT

    public:
        GetPropCommand(QObject *parent = 0);
        virtual ~GetPropCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // GETPROPCOMMAND_H
