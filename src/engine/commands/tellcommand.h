#ifndef TELLCOMMAND_H
#define TELLCOMMAND_H

#include "command.h"


class TellCommand : public Command {

    Q_OBJECT

    public:
        TellCommand(QObject *parent = 0);
        virtual ~TellCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // TELLCOMMAND_H
