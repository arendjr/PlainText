#ifndef ROOMSLISTCOMMAND_H
#define ROOMSLISTCOMMAND_H

#include "apicommand.h"


class RoomsListCommand : public ApiCommand {

    public:
        RoomsListCommand(QObject *parent = 0);
        virtual ~RoomsListCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // ROOMSLISTCOMMAND_H
