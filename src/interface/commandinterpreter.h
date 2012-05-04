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

        void say(const QString &sentence);

        void slashMe(const QString &sentence);

        void enterArea(const GameObjectPtr &area);
        void leaveArea(const GameObjectPtr &area, const QString &exitName = QString::null);

    signals:
        void exit();

    protected:
        const GameObjectPtr &localObjectByName(const QString &objectName) const;

        void getProperty(const GameObjectPtr &object, const QString &propertyName);
        void setProperty(const GameObjectPtr &object, const QString &propertyName, const QString &value);

        void addExit(const QString &exitName, const QString &destinationAreaId, bool hidden);
        void removeExit(const QString &exitName);

    private:
        Character *m_character;
};

#endif // COMMANDINTERPRETER_H
