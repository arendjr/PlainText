#include "gameobjectptr.h"

#include <algorithm>
#include <climits>
#include <cstring>
#include <utility>

#include <QDebug>
#include <QMetaProperty>
#include <QStringList>
#include <QVector>

#include "gameobject.h"
#include "item.h"
#include "realm.h"
#include "util.h"


GameObjectPtr::GameObjectPtr() :
    m_gameObject(nullptr),
    m_id(0),
    m_list(nullptr) {

    m_objectType[0] = '\0';
}

GameObjectPtr::GameObjectPtr(GameObject *gameObject) :
    GameObjectPtr() {

    if (gameObject) {
        m_gameObject = gameObject;
        strcpy(m_objectType, gameObject->objectType());
        m_id = gameObject->id() ? gameObject->id() : UINT_MAX;
        m_gameObject->registerPointer(this);
    }
}

GameObjectPtr::GameObjectPtr(Realm *realm, const char *objectType, uint id) :
    GameObjectPtr() {

    m_id = id;

    if (objectType) {
        strcpy(m_objectType, objectType);
    }

    if (realm->isInitialized()) {
        resolve(realm);
    }
}

GameObjectPtr::GameObjectPtr(const GameObjectPtr &other) :
    m_gameObject(other.m_gameObject),
    m_id(other.m_id),
    m_list(nullptr) {

    strcpy(m_objectType, other.m_objectType);

    if (m_gameObject) {
        m_gameObject->registerPointer(this);
    }
}

GameObjectPtr::GameObjectPtr(GameObjectPtr &&other) :
    GameObjectPtr() {

    *this = std::move(other);
}

GameObjectPtr::~GameObjectPtr() {

    if (m_gameObject) {
        m_gameObject->unregisterPointer(this);
    }
}

bool GameObjectPtr::isNull() const {

    return m_id == 0;
}

GameObjectPtr &GameObjectPtr::operator=(const GameObjectPtr &other) {

    if (&other != this) {
        if (m_gameObject) {
            m_gameObject->unregisterPointer(this);
        }

        m_gameObject = other.m_gameObject;
        strcpy(m_objectType, other.m_objectType);
        m_id = other.m_id;

        if (m_gameObject) {
            m_gameObject->registerPointer(this);
        }
    }

    if (m_list && m_id == 0) {
        m_list->removeOne(*this);
    }

    return *this;
}

GameObjectPtr &GameObjectPtr::operator=(GameObjectPtr &&other) {

    if (&other != this) {
        swap(*this, other);
    }

    return *this;
}

bool GameObjectPtr::operator==(const GameObjectPtr &other) const {

    return m_id == other.m_id && m_gameObject == other.m_gameObject;
}

bool GameObjectPtr::operator==(const GameObject *other) const {

    Q_ASSERT(other);
    return m_gameObject == other;
}

bool GameObjectPtr::operator!=(const GameObjectPtr &other) const {

    return m_id != other.m_id || m_gameObject != other.m_gameObject;
}

bool GameObjectPtr::operator!=(const GameObject *other) const {

    Q_ASSERT(other);
    return m_gameObject != other;
}

void GameObjectPtr::resolve(Realm *realm) {

    if (m_id == 0) {
        return;
    }

    m_gameObject = realm->getObject(m_objectType[0] == '\0' ? nullptr : m_objectType, m_id);
    if (!m_gameObject) {
        throw GameException(GameException::InvalidGameObjectPointer, m_objectType, m_id);
    }
    if (m_objectType[0] == '\0') {
        strcpy(m_objectType, m_gameObject->objectType());
    }

    m_gameObject->registerPointer(this);
}

void GameObjectPtr::unresolve(bool unregister) {

    if (m_gameObject) {
        if (unregister) {
            m_gameObject->unregisterPointer(this);
        }
        m_gameObject = nullptr;
    }
}

GameObjectPtr GameObjectPtr::copyUnresolved() const {

    GameObjectPtr copy;
    copy.m_id = m_id;
    strcpy(copy.m_objectType, m_objectType);
    return copy;
}

void GameObjectPtr::setOwnerList(GameObjectPtrList *list) {

    m_list = list;
}

QString GameObjectPtr::toString() const {

    if (m_id == 0) {
        return "0";
    }

    return QString("%1:%2").arg(m_objectType).arg(m_id);
}

GameObjectPtr GameObjectPtr::fromString(Realm *realm, const QString &string) {

    if (string == "0") {
        return GameObjectPtr();
    }

    QStringList components = string.split(':');
    if (components.length() != 2) {
        throw GameException(GameException::InvalidGameObjectPointer);
    }

    return GameObjectPtr(realm, components[0].toAscii().constData(), components[1].toInt());
}

void swap(GameObjectPtr &first, GameObjectPtr &second) {

    if (first.m_gameObject && first.m_gameObject == second.m_gameObject) {
        return;
    }

    if (first.m_gameObject) {
        first.m_gameObject->registerPointer(&second);
        first.m_gameObject->unregisterPointer(&first);
    }
    if (second.m_gameObject) {
        second.m_gameObject->registerPointer(&first);
        second.m_gameObject->unregisterPointer(&second);
    }

    std::swap(first.m_gameObject, second.m_gameObject);
    std::swap(first.m_objectType, second.m_objectType);
    std::swap(first.m_id, second.m_id);

    if (first.m_list && first.m_id == 0) {
        first.m_list->removeOne(first);
    }
    if (second.m_list && second.m_id == 0) {
        second.m_list->removeOne(second);
    }
}

void swapWithinList(GameObjectPtr &first, GameObjectPtr &second) {

    if (second.m_gameObject) {
        second.m_gameObject->registerPointer(&first);
        second.m_gameObject->unregisterPointer(&second);
    }

    std::swap(first.m_gameObject, second.m_gameObject);
    std::swap(first.m_objectType, second.m_objectType);
    std::swap(first.m_id, second.m_id);
}

QScriptValue GameObjectPtr::toScriptValue(QScriptEngine *engine, const GameObjectPtr &pointer) {

    if (pointer.m_gameObject) {
        return engine->newQObject(pointer.m_gameObject, QScriptEngine::QtOwnership,
                                  QScriptEngine::ExcludeDeleteLater |
                                  QScriptEngine::PreferExistingWrapperObject);
    } else {
        return engine->nullValue();
    }
}

void GameObjectPtr::fromScriptValue(const QScriptValue &object, GameObjectPtr &pointer) {

    if (object.isQObject()) {
        pointer = GameObjectPtr(Realm::instance(), nullptr, object.property("id").toUInt32());
    } else {
        pointer = GameObjectPtr();
    }
}


GameObjectPtrList::iterator::iterator() :
    m_list(nullptr),
    m_index(0) {
}

GameObjectPtrList::iterator::iterator(const GameObjectPtrList::iterator &other) :
    m_list(other.m_list),
    m_index(other.m_index) {
}

bool GameObjectPtrList::iterator::operator!=(const GameObjectPtrList::iterator &other) const {

    return m_list != other.m_list || m_index != other.m_index;
}

bool GameObjectPtrList::iterator::operator!=(const GameObjectPtrList::const_iterator &other) const {

    return m_list != other.m_list || m_index != other.m_index;
}

GameObjectPtr &GameObjectPtrList::iterator::operator*() const {

    Q_ASSERT(m_list);
    return m_list->m_items[m_index];
}

GameObjectPtrList::iterator &GameObjectPtrList::iterator::operator++() {

    if (m_index < m_list->m_size - 1 || !m_list->m_nextList) {
        m_index++;
    } else {
        m_list = m_list->m_nextList;
        m_index = 0;
    }
    return *this;
}

GameObjectPtrList::iterator GameObjectPtrList::iterator::operator++(int) {

    GameObjectPtrList::iterator it(*this);
    if (m_index < m_list->m_size - 1 || !m_list->m_nextList) {
        m_index++;
    } else {
        m_list = m_list->m_nextList;
        m_index = 0;
    }
    return it;
}

bool GameObjectPtrList::iterator::operator==(const GameObjectPtrList::iterator &other) const {

    return m_list == other.m_list && m_index == other.m_index;
}

bool GameObjectPtrList::iterator::operator==(const GameObjectPtrList::const_iterator &other) const {

    return m_list == other.m_list && m_index == other.m_index;
}


GameObjectPtrList::const_iterator::const_iterator() :
    m_list(nullptr),
    m_index(0) {
}

GameObjectPtrList::const_iterator::const_iterator(const GameObjectPtrList::const_iterator &other) :
    m_list(other.m_list),
    m_index(other.m_index) {
}

GameObjectPtrList::const_iterator::const_iterator(const GameObjectPtrList::iterator &other) :
    m_list(other.m_list),
    m_index(other.m_index) {
}

bool GameObjectPtrList::const_iterator::operator!=(const GameObjectPtrList::const_iterator &other)
    const {

    return m_list != other.m_list || m_index != other.m_index;
}

const GameObjectPtr &GameObjectPtrList::const_iterator::operator*() const {

    Q_ASSERT(m_list);
    return m_list->m_items[m_index];
}

GameObjectPtrList::const_iterator &GameObjectPtrList::const_iterator::operator++() {

    if (m_index < m_list->m_size - 1 || !m_list->m_nextList) {
        m_index++;
    } else {
        m_list = m_list->m_nextList;
        m_index = 0;
    }
    return *this;
}

GameObjectPtrList::const_iterator GameObjectPtrList::const_iterator::operator++(int) {

    GameObjectPtrList::const_iterator it(*this);
    if (m_index < m_list->m_size - 1 || !m_list->m_nextList) {
        m_index++;
    } else {
        m_list = m_list->m_nextList;
        m_index = 0;
    }
    return it;
}

bool GameObjectPtrList::const_iterator::operator==(const GameObjectPtrList::const_iterator &other)
    const {

    return m_list == other.m_list && m_index == other.m_index;
}


GameObjectPtrList::GameObjectPtrList() :
    m_size(0),
    m_nextList(nullptr) {

    for (int i = 0; i < NUM_ITEMS; i++) {
        m_items[i].setOwnerList(this);
    }
}

GameObjectPtrList::GameObjectPtrList(const GameObjectPtrList &other) :
    GameObjectPtrList() {

    m_size = other.m_size;

    for (int i = 0; i < m_size; i++) {
        m_items[i] = other.m_items[i];
    }

    if (other.m_nextList) {
        m_nextList = new GameObjectPtrList(*other.m_nextList);
    }
}

GameObjectPtrList::GameObjectPtrList(GameObjectPtrList &&other) :
    GameObjectPtrList() {

    *this = std::move(other);
}

GameObjectPtrList::~GameObjectPtrList() {

    delete m_nextList;
}

void GameObjectPtrList::append(const GameObjectPtr &value) {

    if (value.isNull()) {
        return;
    }

    if (m_size < NUM_ITEMS) {
        m_items[m_size] = value;
        m_size++;
    } else {
        if (!m_nextList) {
            m_nextList = new GameObjectPtrList();
        }

        m_nextList->append(value);
    }
}

void GameObjectPtrList::append(const GameObjectPtrList &value) {

    for (const GameObjectPtr &item : value) {
        append(item);
    }
}

GameObjectPtrList::iterator GameObjectPtrList::begin() {

    GameObjectPtrList::iterator it;
    it.m_list = this;
    return it;
}

GameObjectPtrList::const_iterator GameObjectPtrList::begin() const {

    GameObjectPtrList::const_iterator it;
    it.m_list = const_cast<GameObjectPtrList *>(this);
    return it;
}

void GameObjectPtrList::clear() {

    delete m_nextList;

    m_size = 0;
    m_nextList = nullptr;
}

GameObjectPtrList::const_iterator GameObjectPtrList::constBegin() const {

    GameObjectPtrList::const_iterator it;
    it.m_list = const_cast<GameObjectPtrList *>(this);
    return it;
}

GameObjectPtrList::const_iterator GameObjectPtrList::constEnd() const {

    if (m_nextList) {
        return m_nextList->constEnd();
    } else {
        GameObjectPtrList::const_iterator it;
        it.m_list = const_cast<GameObjectPtrList *>(this);
        it.m_index = m_size;
        return it;
    }
}

bool GameObjectPtrList::contains(const GameObjectPtr &value) const {

    for (int i = 0; i < m_size; i++) {
        if (m_items[i] == value) {
            return true;
        }
    }

    if (m_nextList) {
        return m_nextList->contains(value);
    } else {
        return false;
    }
}

GameObjectPtrList::iterator GameObjectPtrList::end() {

    if (m_nextList) {
        return m_nextList->end();
    } else {
        GameObjectPtrList::iterator it;
        it.m_list = this;
        it.m_index = m_size;
        return it;
    }
}

GameObjectPtrList::const_iterator GameObjectPtrList::end() const {

    if (m_nextList) {
        return m_nextList->end();
    } else {
        GameObjectPtrList::const_iterator it;
        it.m_list = const_cast<GameObjectPtrList *>(this);
        it.m_index = m_size;
        return it;
    }
}

GameObjectPtr &GameObjectPtrList::first() {

    return m_items[0];
}

const GameObjectPtr &GameObjectPtrList::first() const {

    return m_items[0];
}

int GameObjectPtrList::indexOf(const GameObjectPtr &value) const {

    for (int i = 0; i < m_size; i++) {
        if (m_items[i] == value) {
            return i;
        }
    }

    if (m_nextList) {
        return m_nextList->indexOf(value) + m_size;
    } else {
        return -1;
    }
}

bool GameObjectPtrList::isEmpty() const {

    return m_size == 0;
}

GameObjectPtr &GameObjectPtrList::last() {

    if (m_nextList) {
        return m_nextList->last();
    } else {
        return m_items[m_size - 1];
    }
}

const GameObjectPtr &GameObjectPtrList::last() const {

    if (m_nextList) {
        return m_nextList->last();
    } else {
        return m_items[m_size - 1];
    }
}

int GameObjectPtrList::length() const {

    if (m_nextList) {
        return m_size + m_nextList->length();
    } else {
        return m_size;
    }
}

int GameObjectPtrList::removeAll(const GameObjectPtr &value) {

    int numRemovals = 0;
    for (int i = 0; i < m_size; i++) {
        if (m_items[i] == value) {
            removeAt(i);
            numRemovals++;
            i--;
        }
    }

    if (m_nextList) {
        numRemovals += m_nextList->removeAll(value);
    }

    return numRemovals;
}

void GameObjectPtrList::removeAt(int i) {

    if (i < m_size) {
        m_items[i].setOwnerList(nullptr);
        m_items[i] = GameObjectPtr();
        for (int j = i; j < m_size - 1; j++) {
            swapWithinList(m_items[j], m_items[j + 1]);
        }
        m_items[i].setOwnerList(this);
        if (m_nextList) {
            m_items[m_size - 1] = m_nextList->m_items[0];
            m_nextList->removeAt(0);
        } else {
            m_size--;
        }
    } else {
        Q_ASSERT(m_nextList);
        m_nextList->removeAt(i - m_size);
    }

    if (m_nextList && m_nextList->m_size == 0) {
        delete m_nextList;
        m_nextList = nullptr;
    }
}

bool GameObjectPtrList::removeOne(const GameObjectPtr &value) {

    for (int i = 0; i < m_size; i++) {
        if (m_items[i] == value) {
            removeAt(i);
            return true;
        }
    }

    if (m_nextList) {
        return m_nextList->removeOne(value);
    } else {
        return false;
    }
}

int GameObjectPtrList::size() const {

    if (m_nextList) {
        return m_size + m_nextList->size();
    } else {
        return m_size;
    }
}

void swap(GameObjectPtrList &first, GameObjectPtrList &second) {

    std::swap(first.m_size, second.m_size);

    for (int i = 0, max = qMax(first.m_size, second.m_size); i < max; i++) {
        swap(first.m_items[i], second.m_items[i]);
    }

    std::swap(first.m_nextList, second.m_nextList);
}

bool GameObjectPtrList::operator!=(const GameObjectPtrList &other) const {

    if (size() != other.size()) {
        return true;
    }

    for (int i = 0; i < m_size; i++) {
        if (m_items[i] != other.m_items[i]) {
            return true;
        }
    }

    if (m_nextList) {
        return *m_nextList != *other.m_nextList;
    } else {
        return false;
    }
}

GameObjectPtrList GameObjectPtrList::operator+(const GameObjectPtrList &other) const {

    GameObjectPtrList list(*this);
    list.append(other);
    return list;
}

GameObjectPtrList &GameObjectPtrList::operator=(const GameObjectPtrList &other) {

    for (int i = other.m_size; i < m_size; i++) {
        m_items[i] = GameObjectPtr();
    }

    m_size = other.m_size;

    for (int i = 0; i < m_size; i++) {
        m_items[i] = other.m_items[i];
    }

    if (m_nextList) {
        delete m_nextList;
    }
    if (other.m_nextList) {
        m_nextList = new GameObjectPtrList(*other.m_nextList);
    } else {
        m_nextList = nullptr;
    }

    return *this;
}

GameObjectPtrList &GameObjectPtrList::operator=(GameObjectPtrList &&other) {

    if (&other != this) {
        swap(*this, other);
    }

    return *this;
}

bool GameObjectPtrList::operator==(const GameObjectPtrList &other) const {

    if (size() != other.size()) {
        return false;
    }

    for (int i = 0; i < m_size; i++) {
        if (m_items[i] != other.m_items[i]) {
            return false;
        }
    }

    if (m_nextList) {
        return *m_nextList == *other.m_nextList;
    } else {
        return true;
    }
}

const GameObjectPtr &GameObjectPtrList::operator[](int i) const {

    if (i < m_size) {
        return m_items[i];
    } else {
        Q_ASSERT(m_nextList);
        return (*m_nextList)[i - m_size];
    }
}

void GameObjectPtrList::resolvePointers(Realm *realm) {

    for (int i = 0; i < m_size; i++) {
        try {
            m_items[i].resolve(realm);
        } catch (const GameException &exception) {
            removeAt(i);
            i--;
        }
    }
    if (m_nextList) {
        m_nextList->resolvePointers(realm);
    }
}

void GameObjectPtrList::unresolvePointers() {

    for (int i = 0; i < m_size; i++) {
        m_items[i].unresolve();
    }
    if (m_nextList) {
        m_nextList->unresolvePointers();
    }
}

GameObjectPtrList GameObjectPtrList::copyUnresolved() const {

    GameObjectPtrList copy;
    copy.m_size = m_size;

    for (int i = 0; i < m_size; i++) {
        copy.m_items[i] = m_items[i].copyUnresolved();
    }

    if (m_nextList) {
        copy.m_nextList = new GameObjectPtrList(m_nextList->copyUnresolved());
    }
    return copy;
}

void GameObjectPtrList::send(const QString &message, int color) const {

    for (int i = 0; i < m_size; i++) {
        m_items[i]->send(message, color);
    }
    if (m_nextList) {
        m_nextList->send(message, color);
    }
}

QString GameObjectPtrList::joinFancy(Options options) const {

    if (isEmpty()) {
        return (options & Capitalized ? "Nothing" : "nothing");
    }

    QVector<GameObject *> objects;
    QStringList objectNames;
    QList<int> objectCounts;

    for (const GameObjectPtr &object : *this) {
        int index = objectNames.indexOf(object->name());
        if (index > -1) {
            objectCounts[index]++;
        } else {
            objects << object.cast<GameObject *>();
            objectNames << object->name();
            objectCounts << 1;
        }
    }

    QStringList strings;
    for (int i = 0; i < objects.size(); i++) {
        const GameObject *object = objects[i];

        if (objectCounts[i] > 1) {
            if (i == 0 && options & Capitalized) {
                strings << Util::capitalize(Util::writtenNumber(objectCounts[i])) + " " +
                           object->plural();
            } else {
                strings << Util::writtenNumber(objectCounts[i]) + " " + object->plural();
            }
        } else {
            if (object->indefiniteArticle().isEmpty()) {
                strings << object->name();
            } else if (options & DefiniteArticles) {
                strings << (i == 0 && options & Capitalized ? "The " : "the ") + object->name();
            } else {
                strings << object->indefiniteName(i == 0 ? (options & Capitalized) : NoOptions);
            }
        }
    }

    return Util::joinFancy(strings);
}
