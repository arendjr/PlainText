#ifndef PORTALSETCOMMAND_H
#define PORTALSETCOMMAND_H

#include "apicommand.h"


class PortalSetCommand : public ApiCommand {

    Q_OBJECT

    public:
        PortalSetCommand(QObject *parent = 0);
        virtual ~PortalSetCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // PORTALSETCOMMAND_H
