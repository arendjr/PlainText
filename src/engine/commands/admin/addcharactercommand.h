#ifndef ADDCHARACTERCOMMAND_H
#define ADDCHARACTERCOMMAND_H

#include "admincommand.h"


class AddCharacterCommand : public AdminCommand {

    Q_OBJECT

    public:
        AddCharacterCommand(QObject *parent = 0);
        virtual ~AddCharacterCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // ADDCHARACTERCOMMAND_H
