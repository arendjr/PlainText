#ifndef DESCRIPTIONCOMMAND_H
#define DESCRIPTIONCOMMAND_H

#include "command.h"


class DescriptionCommand : public Command {

    Q_OBJECT

    public:
        DescriptionCommand(Player *player, QObject *parent = 0);
        virtual ~DescriptionCommand();

        virtual void execute(const QString &command);
};

#endif // DESCRIPTIONCOMMAND_H
