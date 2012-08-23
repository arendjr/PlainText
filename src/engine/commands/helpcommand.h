#ifndef HELPCOMMAND_H
#define HELPCOMMAND_H

#include <QMap>

#include "command.h"


class HelpCommand : public Command {

    Q_OBJECT

    public:
        HelpCommand(Player *player, const QMap<QString, Command *> &commands, QObject *parent = 0);
        virtual ~HelpCommand();

        virtual void execute(const QString &command);

    private:
        const QMap<QString, Command *> &m_commands;
        QMap<QString, QString> m_triggers;

        QString showAdminHelp(const QString &commandName);

        static QString formatColumns(const QStringList &commandNames);
};

#endif // HELPCOMMAND_H
