#ifndef SEARCHCOMMAND_H
#define SEARCHCOMMAND_H

#include "command.h"


class SearchCommand : public Command {

    Q_OBJECT

    public:
        SearchCommand(QObject *parent = 0);
        virtual ~SearchCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // SEARCHCOMMAND_H
