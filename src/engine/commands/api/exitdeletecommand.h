#ifndef EXITDELETECOMMAND_H
#define EXITDELETECOMMAND_H

#include "apicommand.h"


class ExitDeleteCommand : public ApiCommand {

    Q_OBJECT

    public:
        ExitDeleteCommand(QObject *parent = 0);
        virtual ~ExitDeleteCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // EXITDELETECOMMAND_H
