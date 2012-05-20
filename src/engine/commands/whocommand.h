#ifndef WHOCOMMAND_H
#define WHOCOMMAND_H

#include "command.h"


class WhoCommand : public Command {

    Q_OBJECT

    public:
        explicit WhoCommand(Player *player, QObject *parent = 0);
        virtual ~WhoCommand();

        virtual void execute(const QString &command);
};

#endif // WHOCOMMAND_H
