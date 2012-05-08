#ifndef QUITCOMMAND_H
#define QUITCOMMAND_H

#include "command.h"


class QuitCommand : public Command {

    Q_OBJECT

    public:
        explicit QuitCommand(Character *character, QObject *parent = 0);
        virtual ~QuitCommand();

        virtual void execute(const QString &command);
};

#endif // QUITCOMMAND_H
