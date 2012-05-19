#ifndef OPENCOMMAND_H
#define OPENCOMMAND_H

#include "command.h"


class OpenCommand : public Command {

    Q_OBJECT

    public:
        explicit OpenCommand(Player *player, QObject *parent = 0);
        virtual ~OpenCommand();

        virtual void execute(const QString &command);
};

#endif // OPENCOMMAND_H
