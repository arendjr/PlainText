#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QPair>
#include <QStringList>

#include "engine/area.h"
#include "engine/gameobjectptr.h"
#include "engine/player.h"


class QRegExp;

class Realm;

class Command : public QObject {

    Q_OBJECT

    public:
        enum Options {
            None = 0x00,
            IfNotLast = 0x01
        };

        Command(Player *player, QObject *parent = 0);
        virtual ~Command();

        const QString &description() const { return m_description; }
        void setDescription(const QString &description);

        virtual void execute(const QString &command) = 0;

    signals:
        void quit();

    protected:
        inline Player *player() { return m_player; }
        inline Area *currentArea() { return m_player->currentArea().cast<Area *>(); }
        inline Realm *realm() { return m_player->realm(); }

        void setCommand(const QString &command);
        void prependWord(const QString &word);
        void appendWord(const QString &word);

        inline int numWordsLeft() const { return m_words.length(); }
        inline bool hasWordsLeft() const { return m_words.length() > 0; }
        bool assertWordsLeft(const QString &noneLeftText);

        QString takeWord(Options options = None);
        QString takeWord(const char *pattern, Options options = None);
        QString takeWord(const QRegExp &pattern, Options options = None);
        GameObjectPtr takeObject(const GameObjectPtrList &pool);
        GameObjectPtrList takeObjects(const GameObjectPtrList &pool);
        QPair<QString, uint> takeObjectsDescription();
        QString takeRest();

        virtual GameObjectPtrList objectsByDescription(const QPair<QString, uint> &description,
                                                       const GameObjectPtrList &pool);

        bool requireSome(const GameObjectPtr &object,
                         const QString &tooFewText);
        bool requireSome(const GameObjectPtrList &objects,
                         const QString &tooFewText);
        bool requireUnique(const GameObjectPtrList &objects,
                           const QString &tooFewText,
                           const QString &tooManyText);

        inline void send(const QString &message) { m_player->send(message); }

    private:
        Player *m_player;

        QString m_description;

        QStringList m_words;
};

#endif // COMMAND_H
