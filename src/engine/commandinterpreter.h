#ifndef COMMANDINTERPRETER_H
#define COMMANDINTERPRETER_H

#include <QObject>


class CommandRegistry;
class Player;

class CommandInterpreter : public QObject {

    Q_OBJECT

    public:
        explicit CommandInterpreter(QObject *parent = 0);
        virtual ~CommandInterpreter();

        void setRegistry(CommandRegistry *registry);

        void execute(Player *player, const QString &command);

    private:
        CommandRegistry *m_registry;
};

#endif // COMMANDINTERPRETER_H
