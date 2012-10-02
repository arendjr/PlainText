#ifndef SETRACECOMMAND_H
#define SETRACECOMMAND_H

#include "admincommand.h"


class SetRaceCommand : public AdminCommand {

    Q_OBJECT

    public:
        SetRaceCommand(QObject *parent = 0);
        virtual ~SetRaceCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // SETRACECOMMAND_H
