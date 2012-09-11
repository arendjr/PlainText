#ifndef COPYTRIGGERSCOMMAND_H
#define COPYTRIGGERSCOMMAND_H

#include "admincommand.h"


class CopyTriggersCommand : public AdminCommand {

    Q_OBJECT

    public:
        CopyTriggersCommand(Player *player, QObject *parent = 0);
        virtual ~CopyTriggersCommand();

        virtual void execute(const QString &command);
};

#endif // COPYTRIGGERSCOMMAND_H
