#include "item.h"


Item::Item(uint id, Options options) :
    GameObject("item", id, options),
    m_portable(false) {
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

Item::Item(const char *objectType, uint id, Options options) :
    GameObject(objectType, id, options),
    m_portable(false) {
}
