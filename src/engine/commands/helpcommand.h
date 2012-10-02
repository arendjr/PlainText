#ifndef HELPCOMMAND_H
#define HELPCOMMAND_H

#include "command.h"


class HelpCommand : public Command {

    Q_OBJECT

    public:
        HelpCommand(QObject *parent = 0);
        virtual ~HelpCommand();

        virtual void execute(Player *player, const QString &command);

    private:
        QString showAdminHelp(const QString &commandName);

        static QString formatColumns(const QStringList &commandNames);
};

#endif // HELPCOMMAND_H
