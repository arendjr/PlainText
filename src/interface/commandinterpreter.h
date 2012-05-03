#ifndef COMMANDINTERPRETER_H
#define COMMANDINTERPRETER_H

#include <QObject>


class Character;
class GameObjectPtr;

class CommandInterpreter : public QObject {

    Q_OBJECT

    public:
        explicit CommandInterpreter(Character *character, QObject *parent = 0);
        virtual ~CommandInterpreter();

        void execute(const QString &command);

        void goExit(const QString &exitName);

        void enterArea(const GameObjectPtr &area);

    signals:
        void write(QString data);

        void exit();

    private:
        Character *m_character;
};

#endif // COMMANDINTERPRETER_H
