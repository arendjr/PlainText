#ifndef COMMANDREGISTRY_H
#define COMMANDREGISTRY_H

#include <QMap>
#include <QObject>
#include <QStringList>


class Command;
class Player;
class QScriptValue;

class CommandRegistry : public QObject {

    Q_OBJECT

    public:
        explicit CommandRegistry(QObject *parent = 0);
        virtual ~CommandRegistry();

        void registerCommand(const QString &commandName, Command *command);
        Q_INVOKABLE void registerCommand(const QString &commandName, const QScriptValue &object);

        void registerAdminCommand(const QString &commandName, Command *command);
        Q_INVOKABLE void registerAdminCommand(const QString &commandName, const QScriptValue &object);

        QStringList commandNames() const { return m_commands.keys(); }
        bool contains(const QString &commandName) const;
        Command *command(const QString &commandName) const;
        QString description(const QString &commandName) const;

        QStringList adminCommandNames() const { return m_adminCommands.keys(); }
        bool adminCommandsContains(const QString &commandName) const;
        Command *adminCommand(const QString &commandName) const;
        QString adminCommandDescription(const QString &commandName) const;

        QStringList apiCommandNames() const { return m_apiCommands.keys(); }
        bool apiCommandsContains(const QString &commandName) const;
        Command *apiCommand(const QString &commandName) const;
        QString apiCommandDescription(const QString &commandName) const;

    private:
        QMap<QString, Command *> m_commands;
        QMap<QString, Command *> m_adminCommands;
        QMap<QString, Command *> m_apiCommands;
};

#endif // COMMANDREGISTRY_H
