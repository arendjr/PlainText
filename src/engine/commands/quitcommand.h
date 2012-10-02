#ifndef QUITCOMMAND_H
#define QUITCOMMAND_H

#include "command.h"


class QuitCommand : public Command {

    Q_OBJECT

    public:
        QuitCommand(QObject *parent = 0);
        virtual ~QuitCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // QUITCOMMAND_H
