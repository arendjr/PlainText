#ifndef COMMANDINTERPRETER_H
#define COMMANDINTERPRETER_H

#include <QMap>
#include <QObject>


class Player;
class Command;

class CommandInterpreter : public QObject {

    Q_OBJECT

    public:
        explicit CommandInterpreter(Player *player);
        virtual ~CommandInterpreter();

        void execute(const QString &command);

    signals:
        void quit();

    private:
        Player *m_player;

        QMap<QString, Command *> m_commands;
};

#endif // COMMANDINTERPRETER_H
