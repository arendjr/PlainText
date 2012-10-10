#ifndef PROPERTYSETCOMMAND_H
#define PROPERTYSETCOMMAND_H

#include "apicommand.h"


class PropertySetCommand : public ApiCommand {

    public:
        PropertySetCommand(QObject *parent = 0);
        virtual ~PropertySetCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // PROPERTYSETCOMMAND_H
