#ifndef ADDCHARACTERCOMMAND_H
#define ADDCHARACTERCOMMAND_H

#include "admincommand.h"


class AddCharacterCommand : public AdminCommand {

    Q_OBJECT

    public:
        AddCharacterCommand(Player *player, QObject *parent = 0);
        virtual ~AddCharacterCommand();

        virtual void execute(const QString &command);
};

#endif // ADDCHARACTERCOMMAND_H
