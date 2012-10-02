#ifndef GTALKCOMMAND_H
#define GTALKCOMMAND_H

#include "command.h"


class GtalkCommand : public Command {

    Q_OBJECT

    public:
        GtalkCommand(QObject *parent = 0);
        virtual ~GtalkCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // GTALKCOMMAND_H
