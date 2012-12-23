#ifndef OBJECTDELETECOMMAND_H
#define OBJECTDELETECOMMAND_H

#include "apicommand.h"


class ObjectDeleteCommand : public ApiCommand {

    Q_OBJECT

    public:
        ObjectDeleteCommand(QObject *parent = 0);
        virtual ~ObjectDeleteCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // OBJECTDELETECOMMAND_H
