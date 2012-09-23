#ifndef DATAGETCOMMAND_H
#define DATAGETCOMMAND_H

#include "apicommand.h"


class DataGetCommand : public ApiCommand {

    public:
        DataGetCommand(Player *player, QObject *parent = 0);
        virtual ~DataGetCommand();

        virtual void execute(const QString &command);
};

#endif // DATAGETCOMMAND_H
