#ifndef AREASLISTCOMMAND_H
#define AREASLISTCOMMAND_H

#include "apicommand.h"


class AreasListCommand : public ApiCommand {

    public:
        AreasListCommand(QObject *parent = 0);
        virtual ~AreasListCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // AREASLISTCOMMAND_H
