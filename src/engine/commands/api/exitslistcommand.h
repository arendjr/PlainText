#ifndef EXITSLISTCOMMAND_H
#define EXITSLISTCOMMAND_H

#include "apicommand.h"


class ExitsListCommand : public ApiCommand {

    public:
        ExitsListCommand(QObject *parent = 0);
        virtual ~ExitsListCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // EXITSLISTCOMMAND_H
