#ifndef SLASHMECOMMAND_H
#define SLASHMECOMMAND_H

#include "command.h"


class SlashMeCommand : public Command {

    Q_OBJECT

    public:
        explicit SlashMeCommand(Character *character, QObject *parent = 0);
        virtual ~SlashMeCommand();

        virtual void execute(const QString &command);
};

#endif // SLASHMECOMMAND_H
