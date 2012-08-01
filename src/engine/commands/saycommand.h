#ifndef SAYCOMMAND_H
#define SAYCOMMAND_H

#include "command.h"


class SayCommand : public Command {

    Q_OBJECT

    public:
        SayCommand(Player *player, QObject *parent = 0);
        virtual ~SayCommand();

        virtual void execute(const QString &command);
};

#endif // SAYCOMMAND_H
