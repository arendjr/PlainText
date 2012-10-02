#ifndef EXITSETCOMMAND_H
#define EXITSETCOMMAND_H

#include "apicommand.h"


class ExitSetCommand : public ApiCommand {

    Q_OBJECT

    public:
        ExitSetCommand(QObject *parent = 0);
        virtual ~ExitSetCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // EXITSETCOMMAND_H
