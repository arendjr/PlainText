#ifndef WHOCOMMAND_H
#define WHOCOMMAND_H

#include "command.h"


class WhoCommand : public Command {

    Q_OBJECT

    public:
        WhoCommand(QObject *parent = 0);
        virtual ~WhoCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // WHOCOMMAND_H
