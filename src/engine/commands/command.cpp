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

void Command::setCommand(const QString &command) {

    m_words = command.trimmed().split(QRegExp("\\s+"));

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

bool Command::assertWordsLeft(const QString &noneLeftText) {

    if (numWordsLeft() == 0) {
        m_player->send(noneLeftText);
        return false;
    } else {
        return true;
    }
}

QString Command::takeWord() {

    if (m_words.length() > 0) {
        return m_words.takeFirst();
    }

    return QString();
}

QString Command::takeWord(const QRegExp &pattern) {

    if (m_words.length() > 0) {
        if (pattern.exactMatch(m_words[0])) {
            return m_words.takeFirst();
        }
    }

    return QString();
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
        foreach (const GameObjectPtr &object, pool) {
            if (object->name().toLower().startsWith(description.first)) {
                objects << object;
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
