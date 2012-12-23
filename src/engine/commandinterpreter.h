#ifndef COMMANDINTERPRETER_H
#define COMMANDINTERPRETER_H

#include <QObject>


class Character;
class CommandRegistry;

class CommandInterpreter : public QObject {

    Q_OBJECT

    public:
        explicit CommandInterpreter(QObject *parent = 0);
        virtual ~CommandInterpreter();

        void setRegistry(CommandRegistry *registry);

        void execute(Character *character, const QString &command);

    private:
        CommandRegistry *m_registry;
};

#endif // COMMANDINTERPRETER_H
