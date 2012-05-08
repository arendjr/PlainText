#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QPair>
#include <QStringList>

#include "engine/area.h"
#include "engine/character.h"
#include "engine/gameobjectptr.h"


class QRegExp;

class Command : public QObject {

    Q_OBJECT

    public:
        explicit Command(Character *character, QObject *parent = 0);
        virtual ~Command();

        virtual void execute(const QString &command) = 0;

    signals:
        void quit();

    protected:
        Character *character() { return m_character; }
        Area *currentArea() { return m_character->currentArea().cast<Area *>(); }

        void setCommand(const QString &command);

        int numWordsLeft() const { return m_words.length(); }
        bool assertWordsLeft(const QString &noneLeftText);

        QString takeWord();
        QString takeWord(const QRegExp &pattern);
        GameObjectPtrList takeObjects(const GameObjectPtrList &pool);
        QPair<QString, uint> takeObjectsDescription();
        QString takeRest();

        virtual GameObjectPtrList objectsByDescription(const QPair<QString, uint> &description,
                                                       const GameObjectPtrList &pool);

        bool requireUnique(const GameObjectPtrList &objects,
                           const QString &tooFewText,
                           const QString &tooManyText);

        void enterArea(const GameObjectPtr &area);
        void leaveArea(const GameObjectPtr &area, const QString &exitName = QString::null);

    private:
        Character *m_character;

        QStringList m_words;
};

#endif // COMMAND_H
