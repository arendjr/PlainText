#ifndef OPENCOMMAND_H
#define OPENCOMMAND_H

#include "command.h"


class OpenCommand : public Command {

    Q_OBJECT

    public:
        OpenCommand(QObject *parent = 0);
        virtual ~OpenCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // OPENCOMMAND_H
