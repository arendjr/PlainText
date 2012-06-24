#include "item.h"

#include "gameobjectptr.h"
#include "util.h"


Item::Item(uint id, Options options) :
    GameObject("item", id, options),
    m_portable(false),
    m_weight(0) {
}

Item::Item(const char *objectType, uint id, Options options) :
    GameObject(objectType, id, options),
    m_portable(false),
    m_weight(0) {
}

Item::~Item() {
}

void Item::setName(const QString &newName) {

    if (name() != newName) {
        if (newName.length() > 0) {
            if (newName.endsWith("y")) {
                m_plural = newName.left(newName.length() - 1) + "ies";
            } else if (newName.endsWith("f")) {
                m_plural = newName.left(newName.length() - 1) + "ves";
            } else if (newName.endsWith("fe")) {
                m_plural = newName.left(newName.length() - 2) + "ves";
            } else if (newName.endsWith("is")) {
                m_plural = newName.left(newName.length() - 2) + "es";
            } else {
                m_plural = newName + "s";
            }

            if (newName[0] == 'a' || newName[0] == 'e' || newName[0] == 'i' ||
                newName[0] == 'o' || newName[0] == 'u') {
                m_indefiniteArticle = "an";
            } else {
                m_indefiniteArticle = "a";
            }
        }

        GameObject::setName(newName);
    }
}

QString Item::definiteName(const GameObjectPtrList &pool, Options options) {

    if (m_indefiniteArticle.isEmpty()) {
        return name();
    } else {
        int position = 0;
        int total = 0;
        gopl_foreach (other, pool) {
            if (other->name() == name()) {
                total++;

                if (other->id() == id()) {
                    position = total;
                }
            }
        }

        return QString(options & Capitalized ? "The " : "the ") +
               QString(total > 1 ? Util::writtenPosition(position) + " ": "") +
               name();
    }
}

QString Item::indefiniteName(Options options) {

    if (m_indefiniteArticle.isEmpty()) {
        return name();
    } else {
        return (options & Capitalized ? Util::capitalize(m_indefiniteArticle) :
                                        m_indefiniteArticle) + " " +
               name();
    }
}

void Item::setPlural(const QString &plural) {

    if (m_plural != plural) {
        m_plural = plural;

        setModified();
    }
}

void Item::setIndefiniteArticle(const QString &indefiniteArticle) {

    if (m_indefiniteArticle != indefiniteArticle) {
        m_indefiniteArticle = indefiniteArticle;

        setModified();
    }
}

void Item::setPortable(bool portable) {

    if (m_portable != portable) {
        m_portable = portable;

        setModified();
    }
}

void Item::adjustWeight(int delta) {

    setWeight(m_weight + delta);
}

void Item::setWeight(int weight) {

    if (m_weight != weight) {
        m_weight = weight;

        setModified();
    }
}
