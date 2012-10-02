#ifndef WIELDCOMMAND_H
#define WIELDCOMMAND_H

#include "command.h"


class WieldCommand : public Command {

    Q_OBJECT

    public:
        WieldCommand(QObject *parent = 0);
        virtual ~WieldCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // WIELDCOMMAND_H
