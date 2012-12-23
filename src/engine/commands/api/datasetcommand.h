#ifndef DATASETCOMMAND_H
#define DATASETCOMMAND_H

#include "apicommand.h"


class DataSetCommand : public ApiCommand {

    public:
        DataSetCommand(QObject *parent = 0);
        virtual ~DataSetCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // DATASETCOMMAND_H
