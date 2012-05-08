#ifndef ADDEXITCOMMAND_H
#define ADDEXITCOMMAND_H

#include "admincommand.h"


class AddExitCommand : public AdminCommand {

    Q_OBJECT

    public:
        explicit AddExitCommand(Character *character, QObject *parent = 0);
        virtual ~AddExitCommand();

        virtual void execute(const QString &command);
};

#endif // ADDEXITCOMMAND_H
