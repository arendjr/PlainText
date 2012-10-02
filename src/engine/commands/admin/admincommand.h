#ifndef ADMINCOMMAND_H
#define ADMINCOMMAND_H

#include "command.h"


class AdminCommand : public Command {

    Q_OBJECT

    public:
        AdminCommand(QObject *parent = 0);
        virtual ~AdminCommand();

    protected:
        virtual GameObjectPtrList objectsByDescription(const QPair<QString, uint> &description,
                                                       const GameObjectPtrList &pool);
};

#endif // ADMINCOMMAND_H
