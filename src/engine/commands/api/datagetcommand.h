#ifndef DATAGETCOMMAND_H
#define DATAGETCOMMAND_H

#include "apicommand.h"


class DataGetCommand : public ApiCommand {

    public:
        DataGetCommand(QObject *parent = 0);
        virtual ~DataGetCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // DATAGETCOMMAND_H
