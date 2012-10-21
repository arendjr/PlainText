#ifndef CLOSECOMMAND_H
#define CLOSECOMMAND_H

#include "command.h"


class CloseCommand : public Command {

    Q_OBJECT

    public:
        CloseCommand(QObject *parent = 0);
        virtual ~CloseCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // CLOSECOMMAND_H
