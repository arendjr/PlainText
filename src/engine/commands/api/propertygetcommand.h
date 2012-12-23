#ifndef PROPERTYGETCOMMAND_H
#define PROPERTYGETCOMMAND_H

#include "apicommand.h"


class PropertyGetCommand : public ApiCommand {

    public:
        PropertyGetCommand(QObject *parent = 0);
        virtual ~PropertyGetCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // PROPERTYGETCOMMAND_H
