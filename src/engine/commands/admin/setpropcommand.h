#ifndef SETPROPCOMMAND_H
#define SETPROPCOMMAND_H

#include "admincommand.h"


class SetPropCommand : public AdminCommand {

    Q_OBJECT

    public:
        explicit SetPropCommand(Character *character, QObject *parent = 0);
        virtual ~SetPropCommand();

        virtual void execute(const QString &command);
};

#endif // SETPROPCOMMAND_H
