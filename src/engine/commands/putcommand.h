#ifndef PUTCOMMAND_H
#define PUTCOMMAND_H

#include "command.h"


class PutCommand : public Command {

    Q_OBJECT

    public:
        PutCommand(QObject *parent = 0);
        virtual ~PutCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // PUTCOMMAND_H
