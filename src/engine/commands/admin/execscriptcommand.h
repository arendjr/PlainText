#ifndef EXECSCRIPTCOMMAND_H
#define EXECSCRIPTCOMMAND_H

#include "admincommand.h"


class ExecScriptCommand : public AdminCommand {

    Q_OBJECT

    public:
        explicit ExecScriptCommand(Character *character, QObject *parent = 0);
        virtual ~ExecScriptCommand();

        virtual void execute(const QString &command);
};

#endif // EXECSCRIPTCOMMAND_H
