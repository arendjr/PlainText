#ifndef COPYITEMCOMMAND_H
#define COPYITEMCOMMAND_H

#include "admincommand.h"


class CopyItemCommand : public AdminCommand {

    Q_OBJECT

    public:
        CopyItemCommand(QObject *parent = 0);
        virtual ~CopyItemCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // COPYITEMCOMMAND_H
