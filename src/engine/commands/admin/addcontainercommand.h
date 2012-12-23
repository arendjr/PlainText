#ifndef ADDCONTAINERCOMMAND_H
#define ADDCONTAINERCOMMAND_H

#include "admincommand.h"


class AddContainerCommand : public AdminCommand {

    Q_OBJECT

    public:
        AddContainerCommand(QObject *parent = 0);
        virtual ~AddContainerCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // ADDCONTAINERCOMMAND_H
