#ifndef OBJECTSETCOMMAND_H
#define OBJECTSETCOMMAND_H

#include "apicommand.h"


class ObjectSetCommand : public ApiCommand {

    Q_OBJECT

    public:
        ObjectSetCommand(QObject *parent = 0);
        virtual ~ObjectSetCommand();

        virtual void execute(Character *character, const QString &command);

    protected:
        virtual void setCommand(const QString &command);
};

#endif // OBJECTSETCOMMAND_H
