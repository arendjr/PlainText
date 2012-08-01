#ifndef CLOSECOMMAND_H
#define CLOSECOMMAND_H

#include "command.h"


class CloseCommand : public Command {

    Q_OBJECT

    public:
        CloseCommand(Player *player, QObject *parent = 0);
        virtual ~CloseCommand();

        virtual void execute(const QString &command);
};

#endif // GOCOMMAND_H
