#ifndef LISTEXITSCOMMAND_H
#define LISTEXITSCOMMAND_H

#include "apicommand.h"


class ListExitsCommand : public ApiCommand {

    public:
        ListExitsCommand(Player *player, QObject *parent = 0);
        virtual ~ListExitsCommand();

        virtual void execute(const QString &command);
};

#endif // LISTEXITSCOMMAND_H
