#ifndef PROPERTYSETCOMMAND_H
#define PROPERTYSETCOMMAND_H

#include "apicommand.h"


class PropertySetCommand : public ApiCommand {

    public:
        PropertySetCommand(QObject *parent = 0);
        virtual ~PropertySetCommand();

        virtual void execute(Character *character, const QString &command);
};

#endif // PROPERTYSETCOMMAND_H
