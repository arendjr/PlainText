#ifndef LOOKCOMMAND_H
#define LOOKCOMMAND_H

#include "command.h"


class LookCommand : public Command {

    Q_OBJECT

    public:
        LookCommand(Player *player, QObject *parent = 0);
        virtual ~LookCommand();

        virtual void execute(const QString &command);
};

#endif // LOOKCOMMAND_H
