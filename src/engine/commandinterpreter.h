#ifndef COMMANDINTERPRETER_H
#define COMMANDINTERPRETER_H

#include <QMap>
#include <QObject>


class Character;
class Command;

class CommandInterpreter : public QObject {

    Q_OBJECT

    public:
        explicit CommandInterpreter(Character *character, QObject *parent = 0);
        virtual ~CommandInterpreter();

        void execute(const QString &command);

    signals:
        void quit();

    private:
        Character *m_character;

        QMap<QString, Command *> m_commands;
};

#endif // COMMANDINTERPRETER_H
