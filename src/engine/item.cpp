#include "item.h"

#include "util.h"


#define super GameObject

Item::Item(Realm *realm, uint id, Options options) :
    Item(realm, "item", id, options) {
}

Item::Item(Realm *realm, const char *objectType, uint id, Options options) :
    super(realm, objectType, id, options),
    m_portable(false),
    m_weight(0),
    m_cost(0.0) {
}

Item::~Item() {
}

QString Item::definiteName(const GameObjectPtrList &pool, Options options) {

    if (m_indefiniteArticle.isEmpty()) {
        return name();
    } else {
        int position = 0;
        int total = 0;
        for (const GameObjectPtr &other : pool) {
            if (other->name() == name()) {
                total++;

                if (other->id() == id()) {
                    position = total;
                }
            }
        }

        return QString(options & Capitalized ? "The " : "the ") +
               (total > 1 ? QString(Util::writtenPosition(position) + " ") : QLatin1String("")) +
               name();
    }
}

QString Item::indefiniteName(Options options) {

    if (m_indefiniteArticle.isEmpty()) {
        return name();
    } else {
        return (options & Capitalized ? Util::capitalize(m_indefiniteArticle) :
                                        m_indefiniteArticle) + " " + name();
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

void Item::setCost(double cost) {

    if (m_cost != cost) {
        m_cost = cost;

        setModified();
    }
}

Item *Item::createGold(Realm *realm, double amount) {

    Item *gold = super::createByObjectType<Item *>(realm, "item");
    gold->setName(QString("$%1 worth of gold").arg(amount));
    gold->setCost(amount);
    gold->setPortable(true);
    return gold;
}

void Item::changeName(const QString &newName) {

    super::changeName(newName);

    int length = newName.length();
    if (length > 0 && !newName.startsWith('$')) {
        if (newName.endsWith("y") && length > 1 && !Util::isVowel(newName[length - 2])) {
            m_plural = newName.left(length - 1) + "ies";
        } else if (newName.endsWith("f")) {
            m_plural = newName.left(length - 1) + "ves";
        } else if (newName.endsWith("fe")) {
            m_plural = newName.left(length - 2) + "ves";
        } else if (newName.endsWith("s") || newName.endsWith("x") ||
                   newName.endsWith("sh") || newName.endsWith("ch")) {
            m_plural = newName.left(length - 2) + "es";
        } else if (newName.endsWith("ese")) {
            m_plural = newName;
        } else {
            m_plural = newName + "s";
        }

        if (Util::isVowel(newName[0])) {
            m_indefiniteArticle = "an";
        } else {
            m_indefiniteArticle = "a";
        }
    }
}
