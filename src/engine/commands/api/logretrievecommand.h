#ifndef LOGRETRIEVECOMMAND_H
#define LOGRETRIEVECOMMAND_H

#include "apicommand.h"


class LogRetrieveCommand : public ApiCommand {

    public:
        LogRetrieveCommand(QObject *parent = 0);
        virtual ~LogRetrieveCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // LOGRETRIEVECOMMAND_H
