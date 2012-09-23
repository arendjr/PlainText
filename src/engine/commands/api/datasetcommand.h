#ifndef DATASETCOMMAND_H
#define DATASETCOMMAND_H

#include "apicommand.h"


class DataSetCommand : public ApiCommand {

    public:
        DataSetCommand(Player *player, QObject *parent = 0);
        virtual ~DataSetCommand();

        virtual void execute(const QString &command);
};

#endif // DATASETCOMMAND_H
