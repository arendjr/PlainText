#include "command.h"

#include <QRegExp>

#include "engine/util.h"


Command::Command(Player *player, QObject *parent) :
    QObject(parent),
    m_player(player) {

    Q_ASSERT(m_player);
}

Command::~Command() {
}

void Command::setDescription(const QString &description) {

    m_description = description;
}

void Command::setCommand(const QString &_command) {

    QRegExp whitespace("\\s+");
    QString command = _command.trimmed();

    m_words = command.split(whitespace);
    if (m_player->isAdmin()) {
        if (m_words[0] == "exec-script") {
            m_words.clear();
            m_words << command.section(whitespace, 0, 0);
            m_words << command.section(whitespace, 1);
            return;
        }
        if (m_words[0] == "get-trigger" || m_words[0] == "set-trigger") {
            m_words.clear();
            m_words << command.section(whitespace, 0, 0);
            m_words << command.section(whitespace, 1, 1);
            m_words << command.section(whitespace, 2, 2);
            if (m_words.last().toInt() > 0) {
                m_words << command.section(whitespace, 3, 3);
                m_words << command.section(whitespace, 4);
            } else {
                m_words << command.section(whitespace, 3);
            }
            return;
        }
    }

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

void Command::prependWord(const QString &word) {

    m_words.prepend(word);
}

void Command::appendWord(const QString &word) {

    m_words.append(word);
}

bool Command::assertWordsLeft(const QString &noneLeftText) {

    if (!hasWordsLeft()) {
        m_player->send(noneLeftText);
        return false;
    } else {
        return true;
    }
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

    QPair<QString, uint> description = takeObjectsDescription();
    if (!description.first.isEmpty()) {
        GameObjectPtrList objects = objectsByDescription(description, pool);
        if (objects.length() > 0) {
            return objects[0];
        }
    }

    return GameObjectPtr();
}

GameObjectPtrList Command::takeObjects(const GameObjectPtrList &pool) {

    QPair<QString, uint> description = takeObjectsDescription();
    if (!description.first.isEmpty()) {
        return objectsByDescription(description, pool);
    }

    return GameObjectPtrList();
}

QPair<QString, uint> Command::takeObjectsDescription() {

    QPair<QString, uint> description;
    if (m_words.length() > 0) {
        description.first = m_words.takeFirst().toLower();
        if (m_words.length() > 0) {
            description.second = m_words.first().toInt();
            if (description.second > 0) {
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

GameObjectPtrList Command::objectsByDescription(const QPair<QString, uint> &description,
                                                const GameObjectPtrList &pool) {

    GameObjectPtrList objects;
    if (description.first == "all") {
        objects = pool;
    } else {
        for (const GameObjectPtr &object : pool) {
            QString loweredName = object->name().toLower();
            if (loweredName == description.first) {
                objects.clear();
                objects << object;
                break;
            }
            for (const QString &word : loweredName.split(' ')) {
                if (word.startsWith(description.first)) {
                    objects << object;
                    break;
                }
            }
        }
    }
    if (description.second > 0) {
        if (description.second <= (uint) objects.length()) {
            GameObjectPtr selected = objects[description.second - 1];
            objects.clear();
            objects << selected;
        } else {
            objects.clear();
        }
    }
    return objects;
}

bool Command::requireSome(const GameObjectPtr &object, const QString &tooFewText) {

    if (object.isNull()) {
        m_player->send(tooFewText);
        return false;
    } else {
        return true;
    }
}

bool Command::requireSome(const GameObjectPtrList &objects,
                          const QString &tooFewText) {

    if (objects.length() == 0) {
        m_player->send(tooFewText);
        return false;
    } else {
        return true;
    }
}

bool Command::requireUnique(const GameObjectPtrList &objects,
                            const QString &tooFewText,
                            const QString &tooManyText) {

    if (objects.length() == 0) {
        m_player->send(tooFewText);
        return false;
    } else if (objects.length() > 1) {
        m_player->send(tooManyText);
        return false;
    } else {
        return true;
    }
}
