#ifndef AREASLISTCOMMAND_H
#define AREASLISTCOMMAND_H

#include "apicommand.h"


class AreasListCommand : public ApiCommand {

    public:
        AreasListCommand(Player *player, QObject *parent = 0);
        virtual ~AreasListCommand();

        virtual void execute(const QString &command);
};

#endif // AREASLISTCOMMAND_H
