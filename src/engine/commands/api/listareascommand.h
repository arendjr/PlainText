#ifndef LISTAREASCOMMAND_H
#define LISTAREASCOMMAND_H

#include "apicommand.h"


class ListAreasCommand : public ApiCommand {

    public:
        ListAreasCommand(Player *player, QObject *parent = 0);
        virtual ~ListAreasCommand();

        virtual void execute(const QString &command);
};

#endif // LISTAREASCOMMAND_H
