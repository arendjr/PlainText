#ifndef SETCLASSCOMMAND_H
#define SETCLASSCOMMAND_H

#include "admincommand.h"


class SetClassCommand : public AdminCommand {

    Q_OBJECT

    public:
        SetClassCommand(Player *player, QObject *parent = 0);
        virtual ~SetClassCommand();

        virtual void execute(const QString &command);
};

#endif // SETCLASSCOMMAND_H
