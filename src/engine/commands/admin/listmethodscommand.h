#ifndef LISTMETHODSCOMMAND_H
#define LISTMETHODSCOMMAND_H

#include "admincommand.h"


class ListMethodsCommand : public AdminCommand {

    Q_OBJECT

    public:
        ListMethodsCommand(QObject *parent = 0);
        virtual ~ListMethodsCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // LISTMETHODSCOMMAND_H
