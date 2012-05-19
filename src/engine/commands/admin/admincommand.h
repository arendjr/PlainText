#ifndef ADMINCOMMAND_H
#define ADMINCOMMAND_H

#include "engine/commands/command.h"


class AdminCommand : public Command {

    Q_OBJECT

    public:
        explicit AdminCommand(Player *player, QObject *parent = 0);
        virtual ~AdminCommand();

    protected:
        virtual GameObjectPtrList objectsByDescription(const QPair<QString, uint> &description,
                                                       const GameObjectPtrList &pool);
};

#endif // ADMINCOMMAND_H
