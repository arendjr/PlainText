#ifndef RELOADSCRIPTSCOMMAND_H
#define RELOADSCRIPTSCOMMAND_H

#include "admincommand.h"


class ReloadScriptsCommand : public AdminCommand {

    Q_OBJECT

    public:
        ReloadScriptsCommand(QObject *parent = 0);
        virtual ~ReloadScriptsCommand();

        virtual void execute(Player *player, const QString &command);
};

#endif // RELOADSCRIPTSCOMMAND_H
