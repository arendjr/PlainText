#ifndef WIELDCOMMAND_H
#define WIELDCOMMAND_H

#include "command.h"


class WieldCommand : public Command {

    Q_OBJECT

    public:
        WieldCommand(Player *player, QObject *parent = 0);
        virtual ~WieldCommand();

        virtual void execute(const QString &command);
};

#endif // WIELDCOMMAND_H
