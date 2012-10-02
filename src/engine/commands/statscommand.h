#ifndef STATSCOMMAND_H
#define STATSCOMMAND_H

#include "command.h"


class StatsCommand : public Command {

    Q_OBJECT

    public:
        StatsCommand(QObject *parent = 0);
        virtual ~StatsCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // STATSCOMMAND_H
