#ifndef OBJECTSLISTCOMMAND_H
#define OBJECTSLISTCOMMAND_H

#include "apicommand.h"


class ObjectsListCommand : public ApiCommand {

    public:
        ObjectsListCommand(QObject *parent = 0);
        virtual ~ObjectsListCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // OBJECTSLISTCOMMAND_H
