#ifndef GIVECOMMAND_H
#define GIVECOMMAND_H

#include "command.h"


class GiveCommand : public Command {

    Q_OBJECT

    public:
        explicit GiveCommand(Player *player, QObject *parent = 0);
        virtual ~GiveCommand();

        virtual void execute(const QString &command);
};

#endif // GIVECOMMAND_H
