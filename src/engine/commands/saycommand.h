#ifndef SAYCOMMAND_H
#define SAYCOMMAND_H

#include "command.h"


class SayCommand : public Command {

    Q_OBJECT

    public:
        SayCommand(QObject *parent = 0);
        virtual ~SayCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // SAYCOMMAND_H
