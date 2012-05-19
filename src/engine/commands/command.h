#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QPair>
#include <QStringList>

#include "engine/area.h"
#include "engine/gameobjectptr.h"
#include "engine/player.h"


class QRegExp;

class Command : public QObject {

    Q_OBJECT

    public:
        explicit Command(Player *player, QObject *parent = 0);
        virtual ~Command();

        virtual void execute(const QString &command) = 0;

    signals:
        void quit();

    protected:
        Player *player() { return m_player; }
        Area *currentArea() { return m_player->currentArea().cast<Area *>(); }

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

        bool requireSome(const GameObjectPtrList &objects,
                         const QString &tooFewText);
        bool requireUnique(const GameObjectPtrList &objects,
                           const QString &tooFewText,
                           const QString &tooManyText);

    private:
        Player *m_player;

        QStringList m_words;
};

#endif // COMMAND_H
