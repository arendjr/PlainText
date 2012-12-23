#ifndef LISTPROPSCOMMAND_H
#define LISTPROPSCOMMAND_H

#include "admincommand.h"


class ListPropsCommand : public AdminCommand {

    Q_OBJECT

    public:
        ListPropsCommand(QObject *parent = 0);
        virtual ~ListPropsCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // LISTPROPSCOMMAND_H
