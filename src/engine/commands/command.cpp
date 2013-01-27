#include "command.h"

#include <QRegExp>

#include "container.h"
#include "portal.h"
#include "util.h"


#define super QObject

Command::Command(QObject *parent) :
    super(parent) {
}

Command::~Command() {
}

void Command::setDescription(const QString &description) {

    m_description = Util::processHighlights(description);
}

void Command::prepareExecute(Character *player, const QString &command) {

    setCharacter(player);
    setCommand(command);

    m_alias = takeWord();
}

void Command::setCommand(const QString &command) {

    static QRegExp whitespace("\\s+");

    m_words = command.split(whitespace);

    for (int i = 0; i < m_words.length(); i++) {
        QString word = m_words[i];
        if (word.startsWith("\"")) {
            while (i < m_words.length() - 1 && !word.endsWith("\"")) {
                word += " " + m_words[i + 1];
                m_words.removeAt(i);
            }
            if (word.endsWith("\"")) {
                m_words[i] = word.mid(1, word.length() - 2);
            } else {
                m_words[i] = word.mid(1);
            }
        }
    }
}

void Command::setWords(const QStringList &words) {

    m_words = words;
}

void Command::prependWord(const QString &word) {

    m_words.prepend(word);
}

void Command::appendWord(const QString &word) {

    m_words.append(word);
}

bool Command::assertWordsLeft(const QString &noneLeftText) {

    if (!hasWordsLeft()) {
        m_character->send(noneLeftText);
        return false;
    } else {
        return true;
    }
}

QString Command::peekWord() const {

    if (m_words.length() > 0) {
        return m_words.first();
    }

    return QString();
}

QString Command::takeWord(Options options) {

    if (m_words.length() > 0) {
        if (options & IfNotLast && m_words.length() == 1) {
            return QString();
        }
        return m_words.takeFirst();
    }

    return QString();
}

QString Command::takeWord(const char *pattern, Options options) {

    return takeWord(QRegExp(QString(pattern)), options);
}

QString Command::takeWord(const QRegExp &pattern, Options options) {

    if (m_words.length() > 0) {
        if (options & IfNotLast && m_words.length() == 1) {
            return QString();
        }
        if (pattern.exactMatch(m_words[0])) {
            return m_words.takeFirst();
        }
    }

    return QString();
}

GameObjectPtr Command::takeObject(const GameObjectPtrList &pool) {

    ObjectDescription description = takeObjectsDescription();
    if (!description.name.isEmpty()) {
        return objectByDescription(description, pool);
    }

    return GameObjectPtr();
}

GameObjectPtrList Command::takeObjects(const GameObjectPtrList &pool) {

    ObjectDescription description = takeObjectsDescription();
    if (!description.name.isEmpty()) {
        return objectsByDescription(description, pool);
    }

    return GameObjectPtrList();
}

Command::ObjectDescription Command::takeObjectsDescription() {

    takeWord("the", IfNotLast);

    ObjectDescription description;
    description.position = 0;

    if (m_words.isEmpty()) {
        return description;
    }

    description.name = m_words.takeFirst().toLower();

    if (description.name.contains('.')) {
        int index = description.name.indexOf('.');
        description.position = description.name.left(index).toInt();
        if (description.position > 0) {
            description.name = description.name.mid(index + 1);
        }
    } else if (!m_words.isEmpty()) {
        description.position = Util::numericPosition(description.name);
        if (description.position > 0) {
            description.name = m_words.takeFirst().toLower();
        } else {
            description.position = m_words.first().toInt();
            if (description.position > 0) {
                m_words.removeFirst();
            }
        }
    }

    return description;
}

QString Command::takeRest() {

    QString rest = m_words.join(" ");
    m_words.clear();
    return rest;
}

GameObjectPtr Command::objectByDescription(const ObjectDescription &description,
                                           const GameObjectPtrList &pool) {

    GameObjectPtrList objects = objectsByDescription(description, pool);
    if (!objects.isEmpty()) {
        return objects[0];
    }

    return GameObjectPtr();
}

GameObjectPtrList Command::objectsByDescription(const ObjectDescription &description,
                                                const GameObjectPtrList &pool) {

    GameObjectPtrList objects;
    if (description.name == "all") {
        objects = pool;
    } else {
        for (const GameObjectPtr &object : pool) {
            QString name;
            if (object->isPortal()) {
                name = object.unsafeCast<Portal *>()->nameFromRoom(currentRoom());
            } else {
                name = object->name();
            }
            QStringList words = name.toLower().split(' ');
            for (const QString &word : words) {
                if (word.startsWith(description.name)) {
                    objects.append(object);
                    break;
                }
            }
        }
    }
    if (description.position > 0) {
        if (description.position <= (int) objects.length()) {
            GameObjectPtr selected = objects[description.position - 1];
            objects.clear();
            objects.append(selected);
        } else {
            objects.clear();
        }
    }
    return objects;
}

bool Command::requireSome(const GameObjectPtr &object, const QString &tooFewText) {

    if (object.isNull()) {
        m_character->send(tooFewText);
        return false;
    } else {
        return true;
    }
}

bool Command::requireSome(const GameObjectPtrList &objects,
                          const QString &tooFewText) {

    if (objects.length() == 0) {
        m_character->send(tooFewText);
        return false;
    } else {
        return true;
    }
}

bool Command::requireUnique(const GameObjectPtrList &objects,
                            const QString &tooFewText,
                            const QString &tooManyText) {

    if (objects.length() == 0) {
        m_character->send(tooFewText);
        return false;
    } else if (objects.length() > 1) {
        m_character->send(tooManyText);
        return false;
    } else {
        return true;
    }
}

void Command::send(const QString &message, const QString &arg1) {

    send(QString(message).arg(arg1));
}

void Command::send(const QString &message, const QString &arg1, const QString &arg2) {

    send(QString(message).arg(arg1, arg2));
}

void Command::setCharacter(Character *character) {

    m_character = character;
}
