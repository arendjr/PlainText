#ifndef LISTPROPSCOMMAND_H
#define LISTPROPSCOMMAND_H

#include "admincommand.h"


class ListPropsCommand : public AdminCommand {

    Q_OBJECT

    public:
        ListPropsCommand(Player *player, QObject *parent = 0);
        virtual ~ListPropsCommand();

        virtual void execute(const QString &command);
};

#endif // LISTPROPSCOMMAND_H
