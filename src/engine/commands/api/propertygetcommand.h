#ifndef PROPERTYGETCOMMAND_H
#define PROPERTYGETCOMMAND_H

#include "apicommand.h"


class PropertyGetCommand : public ApiCommand {

    public:
        PropertyGetCommand(Player *player, QObject *parent = 0);
        virtual ~PropertyGetCommand();

        virtual void execute(const QString &command);
};

#endif // PROPERTYGETCOMMAND_H
