#ifndef LISTTRIGGERSCOMMAND_H
#define LISTTRIGGERSCOMMAND_H

#include "apicommand.h"


class ListTriggersCommand : public ApiCommand {

    public:
        ListTriggersCommand(Player *player, QObject *parent = 0);
        virtual ~ListTriggersCommand();

        virtual void execute(const QString &command);
};

#endif // LISTTRIGGERSCOMMAND_H
