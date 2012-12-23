#ifndef SETPROPCOMMAND_H
#define SETPROPCOMMAND_H

#include "admincommand.h"


class SetPropCommand : public AdminCommand {

    Q_OBJECT

    public:
        SetPropCommand(QObject *parent = 0);
        virtual ~SetPropCommand();

        virtual void execute(Character *character, const QString &command);

    protected:
        virtual void setCommand(const QString &command);
};

#endif // SETPROPCOMMAND_H
