#ifndef EXECSCRIPTCOMMAND_H
#define EXECSCRIPTCOMMAND_H

#include "admincommand.h"


class ExecScriptCommand : public AdminCommand {

    Q_OBJECT

    public:
        ExecScriptCommand(QObject *parent = 0);
        virtual ~ExecScriptCommand();

        virtual void execute(Character *character, const QString &command);

    protected:
        virtual void setCommand(const QString &command);
};

#endif // EXECSCRIPTCOMMAND_H
