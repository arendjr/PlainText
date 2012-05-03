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
        void leaveArea(const GameObjectPtr &area, const QString &exitName = QString::null);

    signals:
        void exit();

    protected:
        void addExit(const QString &exitName, uint destinationAreaId, bool hidden);
        void removeExit(const QString &exitName);

    private:
        Character *m_character;
};

#endif // COMMANDINTERPRETER_H
