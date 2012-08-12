#ifndef SEARCHCOMMAND_H
#define SEARCHCOMMAND_H

#include "command.h"


class SearchCommand : public Command {

    Q_OBJECT

    public:
        SearchCommand(Player *player, QObject *parent = 0);
        virtual ~SearchCommand();

        virtual void execute(const QString &command);
};

#endif // SEARCHCOMMAND_H
