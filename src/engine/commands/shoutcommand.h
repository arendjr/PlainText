#ifndef SHOUTCOMMAND_H
#define SHOUTCOMMAND_H

#include "command.h"


class ShoutCommand : public Command {

    Q_OBJECT

    public:
        ShoutCommand(QObject *parent = 0);
        virtual ~ShoutCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // SHOUTCOMMAND_H
