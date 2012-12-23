#ifndef COPYTRIGGERSCOMMAND_H
#define COPYTRIGGERSCOMMAND_H

#include "admincommand.h"


class CopyTriggersCommand : public AdminCommand {

    Q_OBJECT

    public:
        CopyTriggersCommand(QObject *parent = 0);
        virtual ~CopyTriggersCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // COPYTRIGGERSCOMMAND_H
