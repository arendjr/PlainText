#ifndef TELLCOMMAND_H
#define TELLCOMMAND_H

#include "command.h"


class TellCommand : public Command {

    Q_OBJECT

    public:
        TellCommand(Player *player, QObject *parent = 0);
        virtual ~TellCommand();

        virtual void execute(const QString &command);
};

#endif // TELLCOMMAND_H
