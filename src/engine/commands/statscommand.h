#ifndef STATSCOMMAND_H
#define STATSCOMMAND_H

#include "command.h"


class StatsCommand : public Command {

    Q_OBJECT

    public:
        explicit StatsCommand(Player *player, QObject *parent = 0);
        virtual ~StatsCommand();

        virtual void execute(const QString &command);
};

#endif // STATSCOMMAND_H
