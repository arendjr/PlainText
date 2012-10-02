#ifndef GIVECOMMAND_H
#define GIVECOMMAND_H

#include "command.h"


class GiveCommand : public Command {

    Q_OBJECT

    public:
        GiveCommand(QObject *parent = 0);
        virtual ~GiveCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // GIVECOMMAND_H
