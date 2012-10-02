#ifndef LOOKCOMMAND_H
#define LOOKCOMMAND_H

#include "command.h"


class LookCommand : public Command {

    Q_OBJECT

    public:
        LookCommand(QObject *parent = 0);
        virtual ~LookCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // LOOKCOMMAND_H
