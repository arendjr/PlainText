#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QPair>
#include <QStringList>

#include "room.h"
#include "gameobjectptr.h"
#include "player.h"


class QRegExp;

class Realm;

class Command : public QObject {

    Q_OBJECT

    public:
        enum Options {
            None = 0x00,
            IfNotLast = 0x01
        };

        Command(QObject *parent = 0);
        virtual ~Command();

        const QString &description() const { return m_description; }
        void setDescription(const QString &description);

        virtual void execute(Player *player, const QString &command) = 0;

    protected:
        virtual void prepareExecute(Player *player, const QString &command);

        inline Player *player() const { return m_player; }
        inline Room *currentRoom() const { return m_player->currentRoom().cast<Room *>(); }
        inline Realm *realm() const { return m_player->realm(); }
        inline const QString &alias() const { return m_alias; }

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
        void setPlayer(Player *player);

        void setCommand(const QString &command);

        QString m_description;

        Player *m_player;

        QStringList m_words;

        QString m_alias;
};

#endif // COMMAND_H
