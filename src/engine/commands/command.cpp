#include "command.h"

#include <QRegExp>

#include "engine/util.h"


Command::Command(Character *character, QObject *parent) :
    QObject(parent),
    m_character(character) {

    Q_ASSERT(m_character);
}

Command::~Command() {
}

void Command::setCommand(const QString &command) {

    m_words = command.trimmed().split(QRegExp("\\s+"));
}

bool Command::assertWordsLeft(const QString &noneLeftText) {

    if (numWordsLeft() == 0) {
        m_character->send(noneLeftText);
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
