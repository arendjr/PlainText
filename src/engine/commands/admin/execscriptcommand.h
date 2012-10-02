#ifndef EXECSCRIPTCOMMAND_H
#define EXECSCRIPTCOMMAND_H

#include "admincommand.h"


class ExecScriptCommand : public AdminCommand {

    Q_OBJECT

    public:
        ExecScriptCommand(QObject *parent = 0);
        virtual ~ExecScriptCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // EXECSCRIPTCOMMAND_H
