#ifndef SHOUTCOMMAND_H
#define SHOUTCOMMAND_H

#include "command.h"


class ShoutCommand : public Command {

    Q_OBJECT

    public:
        ShoutCommand(Player *player, QObject *parent = 0);
        virtual ~ShoutCommand();

        virtual void execute(const QString &command);
};

#endif // SHOUTCOMMAND_H
