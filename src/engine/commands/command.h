#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QPair>
#include <QStringList>

#include "character.h"
#include "constants.h"
#include "room.h"
#include "gameobjectptr.h"


class QRegExp;

class Realm;

class Command : public QObject {

    Q_OBJECT

    public:
        typedef struct {
            QString name;
            int position;
        } ObjectDescription;

        Command(QObject *parent = 0);
        virtual ~Command();

        const QString &description() const { return m_description; }
        void setDescription(const QString &description);

        virtual void execute(Character *character, const QString &command) = 0;

    protected:
        virtual void prepareExecute(Character *character, const QString &command);

        inline Character *character() const { return m_character; }
        inline Room *currentRoom() const { return m_character->currentRoom().cast<Room *>(); }
        inline Realm *realm() const { return m_character->realm(); }
        inline const QString &alias() const { return m_alias; }

        virtual void setCommand(const QString &command);

        void setWords(const QStringList &words);
        void prependWord(const QString &word);
        void appendWord(const QString &word);

        inline int numWordsLeft() const { return m_words.length(); }
        inline bool hasWordsLeft() const { return m_words.length() > 0; }
        bool assertWordsLeft(const QString &noneLeftText);

        QString peekWord() const;
        QString takeWord(Options options = NoOptions);
        QString takeWord(const char *pattern, Options options = NoOptions);
        QString takeWord(const QRegExp &pattern, Options options = NoOptions);
        GameObjectPtr takeObject(const GameObjectPtrList &pool);
        GameObjectPtrList takeObjects(const GameObjectPtrList &pool);
        ObjectDescription takeObjectsDescription();
        QString takeRest();

        GameObjectPtr objectByDescription(const ObjectDescription &description,
                                          const GameObjectPtrList &pool);
        virtual GameObjectPtrList objectsByDescription(const ObjectDescription &description,
                                                       const GameObjectPtrList &pool);

        bool requireSome(const GameObjectPtr &object,
                         const QString &tooFewText);
        bool requireSome(const GameObjectPtrList &objects,
                         const QString &tooFewText);
        bool requireUnique(const GameObjectPtrList &objects,
                           const QString &tooFewText,
                           const QString &tooManyText);

        inline void send(const QString &message) { m_character->send(message); }
        void send(const QString &message, const QString &arg1);
        void send(const QString &message, const QString &arg1, const QString &arg2);

    private:
        void setCharacter(Character *character);

        QString m_description;

        Character *m_character;

        QStringList m_words;

        QString m_alias;
};

#endif // COMMAND_H
