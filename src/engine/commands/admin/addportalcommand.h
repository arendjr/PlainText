#ifndef ADDPORTALCOMMAND_H
#define ADDPORTALCOMMAND_H

#include "admincommand.h"


class AddPortalCommand : public AdminCommand {

    Q_OBJECT

    public:
        AddPortalCommand(QObject *parent = 0);
        virtual ~AddPortalCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // ADDPORTALCOMMAND_H
