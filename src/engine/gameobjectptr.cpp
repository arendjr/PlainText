#include "gameobjectptr.h"

#include <algorithm>
#include <climits>
#include <utility>

#include <QDebug>
#include <QMetaProperty>
#include <QStringList>
#include <QVector>

#include "conversionutil.h"
#include "realm.h"
#include "util.h"


GameObjectPtr::GameObjectPtr() :
    m_gameObject(nullptr),
    m_objectType(GameObjectType::Unknown),
    m_id(0),
    m_list(nullptr) {
}

GameObjectPtr::GameObjectPtr(GameObject *gameObject) :
    GameObjectPtr() {

    if (gameObject) {
        m_gameObject = gameObject;
        m_objectType = gameObject->objectType();
        m_id = gameObject->id() ? gameObject->id() : UINT_MAX;
        m_gameObject->registerPointer(this);
    }
}

GameObjectPtr::GameObjectPtr(Realm *realm, GameObjectType objectType, uint id) :
    m_gameObject(nullptr),
    m_objectType(objectType),
    m_id(id),
    m_list(nullptr) {

    if (realm->isInitialized()) {
        resolve(realm);
    }
}

GameObjectPtr::GameObjectPtr(const GameObjectPtr &other) :
    m_gameObject(other.m_gameObject),
    m_objectType(other.m_objectType),
    m_id(other.m_id),
    m_list(nullptr) {

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
        m_objectType = other.m_objectType;
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

    if (other) {
        return m_gameObject == other;
    } else {
        throw new GameException(GameException::NullPointerReference);
    }
}

bool GameObjectPtr::operator!=(const GameObjectPtr &other) const {

    return m_id != other.m_id || m_gameObject != other.m_gameObject;
}

bool GameObjectPtr::operator!=(const GameObject *other) const {

    if (other) {
        return m_gameObject != other;
    } else {
        throw new GameException(GameException::NullPointerReference);
    }
}

void GameObjectPtr::resolve(Realm *realm) {

    if (m_id == 0) {
        return;
    }

    m_gameObject = realm->getObject(m_objectType, m_id);
    if (!m_gameObject) {
        throw GameException(GameException::InvalidGameObjectPointer, m_objectType, m_id);
    }
    if (m_objectType == GameObjectType::Unknown) {
        m_objectType = m_gameObject->objectType();
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
    copy.m_objectType = m_objectType;
    return copy;
}

void GameObjectPtr::setOwnerList(GameObjectPtrList *list) {

    m_list = list;
}

QString GameObjectPtr::toString() const {

    if (m_id == 0) {
        return "0";
    }

    return QString("%1:%2").arg(m_objectType.toString()).arg(m_id).toLower();
}

QString GameObjectPtr::toUserString(const GameObjectPtr &pointer) {

    if (pointer.isNull()) {
        return "(not set)";
    } else {
        return pointer.toString() + " (" + pointer->name() + ")";
    }
}

void GameObjectPtr::fromUserString(const QString &string, GameObjectPtr &pointer) {

    if (string == "0") {
        return;
    }

    QStringList components = string.split(':');
    if (components.length() != 2) {
        throw GameException(GameException::InvalidGameObjectPointer);
    }

    pointer = GameObjectPtr(Realm::instance(), GameObjectType::fromString(components[0]),
                            components[1].toInt());
}

QString GameObjectPtr::toJsonString(const GameObjectPtr &pointer, Options options) {

    Q_UNUSED(options)

    return ConversionUtil::jsString(pointer.toString());
}

void GameObjectPtr::fromVariant(const QVariant &variant, GameObjectPtr &pointer) {

    QString string = variant.toString();
    if (string == "0") {
        return;
    }

    QStringList components = string.split(':');
    if (components.length() != 2) {
        throw GameException(GameException::InvalidGameObjectPointer);
    }

    QString objectType = Util::capitalize(components[0]);
    pointer = GameObjectPtr(Realm::instance(), GameObjectType::fromString(objectType),
                            components[1].toInt());
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
        pointer = GameObjectPtr(Realm::instance(), GameObjectType::Unknown,
                                object.property("id").toUInt32());
    } else {
        pointer = GameObjectPtr();
    }
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


bool GameObjectPtrList::iterator::operator!=(const GameObjectPtrList::iterator &other) const {

    return m_list != other.m_list || m_index != other.m_index;
}

bool GameObjectPtrList::iterator::operator!=(const GameObjectPtrList::const_iterator &other) const {

    return m_list != other.m_list || m_index != other.m_index;
}

GameObjectPtr &GameObjectPtrList::iterator::operator*() const {

    if (!m_list) {
        throw GameException(GameException::NullIteratorReference);
    }
    return m_list->m_items[m_index];
}

GameObjectPtrList::iterator &GameObjectPtrList::iterator::operator++() {

    m_index++;
    if (m_index == m_list->m_size && m_list->m_nextList) {
        m_list = m_list->m_nextList;
        m_index = 0;
    }
    return *this;
}

GameObjectPtrList::iterator GameObjectPtrList::iterator::operator++(int) {

    GameObjectPtrList::iterator it(*this);
    m_index++;
    if (m_index == m_list->m_size && m_list->m_nextList) {
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


GameObjectPtrList::const_iterator::const_iterator(const GameObjectPtrList::iterator &other) :
    m_list(other.m_list),
    m_index(other.m_index) {
}

bool GameObjectPtrList::const_iterator::operator!=(const GameObjectPtrList::const_iterator &other)
    const {

    return m_list != other.m_list || m_index != other.m_index;
}

const GameObjectPtr &GameObjectPtrList::const_iterator::operator*() const {

    if (!m_list) {
        throw GameException(GameException::NullIteratorReference);
    }
    return m_list->m_items[m_index];
}

GameObjectPtrList::const_iterator &GameObjectPtrList::const_iterator::operator++() {

    m_index++;
    if (m_index == m_list->m_size && m_list->m_nextList) {
        m_list = m_list->m_nextList;
        m_index = 0;
    }
    return *this;
}

GameObjectPtrList::const_iterator GameObjectPtrList::const_iterator::operator++(int) {

    GameObjectPtrList::const_iterator it(*this);
    m_index++;
    if (m_index == m_list->m_size && m_list->m_nextList) {
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
    m_capacity(0),
    m_items(nullptr),
    m_nextList(nullptr) {
}

GameObjectPtrList::GameObjectPtrList(int size) :
    m_size(0),
    m_capacity(size),
    m_items(nullptr),
    m_nextList(nullptr) {

    if (m_capacity) {
        m_items = new GameObjectPtr[m_capacity];

        for (int i = 0; i < m_capacity; i++) {
            m_items[i].setOwnerList(this);
        }
    }
}

GameObjectPtrList::GameObjectPtrList(const GameObjectPtrList &other) :
    m_size(other.length()),
    m_capacity(other.length()),
    m_items(nullptr),
    m_nextList(nullptr) {

    if (m_capacity) {
        m_items = new GameObjectPtr[m_capacity];

        int index = 0;
        for (const GameObjectPtr &pointer : other) {
            m_items[index] = pointer;
            m_items[index].setOwnerList(this);
            index++;
        }
    }
}

GameObjectPtrList::GameObjectPtrList(GameObjectPtrList &&other) :
    GameObjectPtrList() {

    *this = std::move(other);
}

GameObjectPtrList::~GameObjectPtrList() {

    delete m_nextList;
    delete[] m_items;
}

void GameObjectPtrList::append(const GameObjectPtr &value) {

    if (value.isNull()) {
        return;
    }

    if (m_capacity) {
        if (m_size < m_capacity) {
            m_items[m_size] = value;
            m_size++;
        } else {
            if (!m_nextList) {
                m_nextList = new GameObjectPtrList(qMax(2 * m_capacity, 16));
            }

            m_nextList->append(value);
        }
    } else {
        m_size = 1;
        m_capacity = 16;

        m_items = new GameObjectPtr[m_capacity];

        for (int i = 0; i < m_capacity; i++) {
            m_items[i].setOwnerList(this);
        }

        m_items[0] = value;
    }
}

void GameObjectPtrList::append(const GameObjectPtrList &value) {

    int numRemaining = value.length();
    if (numRemaining == 0) {
        return;
    }

    if (!m_capacity) {
        m_capacity = qMax(numRemaining, 16);

        m_items = new GameObjectPtr[m_capacity];

        for (int i = 0; i < m_capacity; i++) {
            m_items[i].setOwnerList(this);
        }
    }

    for (const GameObjectPtr &item : value) {
        if (m_size == m_capacity) {
            if (!m_nextList) {
                m_nextList = new GameObjectPtrList(qMax(2 * m_capacity, numRemaining));
            }

            m_nextList->append(item);
        } else {
            append(item);
            numRemaining--;
        }
    }
}

GameObjectPtrList::iterator GameObjectPtrList::begin() {

    GameObjectPtrList::iterator it;
    it.m_list = this;
    it.m_index = 0;
    return it;
}

GameObjectPtrList::const_iterator GameObjectPtrList::begin() const {

    GameObjectPtrList::const_iterator it;
    it.m_list = const_cast<GameObjectPtrList *>(this);
    it.m_index = 0;
    return it;
}

void GameObjectPtrList::clear() {

    delete m_nextList;
    delete[] m_items;

    m_size = 0;
    m_capacity = 0;
    m_items = nullptr;
    m_nextList = nullptr;
}

GameObjectPtrList::const_iterator GameObjectPtrList::constBegin() const {

    GameObjectPtrList::const_iterator it;
    it.m_list = const_cast<GameObjectPtrList *>(this);
    it.m_index = 0;
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
        int index = m_nextList->indexOf(value);
        if (index != -1) {
            index += m_size;
        }
        return index;
    } else {
        return -1;
    }
}

void GameObjectPtrList::insert(const GameObjectPtr &value) {

    for (int i = 0; i < m_size; i++) {
        if (m_items[i] == value) {
            return;
        }
    }

    if (m_nextList) {
        m_nextList->insert(value);
    } else {
        append(value);
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
        if (m_nextList) {
            m_nextList->removeAt(i - m_size);
        } else {
            throw GameException(GameException::IndexOutOfBounds,
                                QString("Index %1 should be within [0,%2)").arg(i).arg(m_size));
        }
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

void GameObjectPtrList::reserve(int size) {

    if (size && !m_capacity) {
        m_capacity = size;
        m_items = new GameObjectPtr[m_capacity];

        for (int i = 0; i < m_capacity; i++) {
            m_items[i].setOwnerList(this);
        }
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
    std::swap(first.m_capacity, second.m_capacity);
    std::swap(first.m_items, second.m_items);
    std::swap(first.m_nextList, second.m_nextList);
}

bool GameObjectPtrList::operator!=(const GameObjectPtrList &other) const {

    if (size() != other.size()) {
        return true;
    }

    for (int i = 0; i < size(); i++) {
        if (operator[](i) != other[i]) {
            return true;
        }
    }

    return false;
}

GameObjectPtrList GameObjectPtrList::operator+(const GameObjectPtrList &other) const {

    GameObjectPtrList list(*this);
    list.append(other);
    return list;
}

GameObjectPtrList &GameObjectPtrList::operator=(const GameObjectPtrList &other) {

    clear();
    append(other);
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

    for (int i = 0; i < size(); i++) {
        if (operator[](i) != other[i]) {
            return false;
        }
    }

    return true;
}

const GameObjectPtr &GameObjectPtrList::operator[](int i) const {

    if (i < m_size) {
        return m_items[i];
    } else {
        if (m_nextList) {
            return (*m_nextList)[i - m_size];
        } else {
            throw GameException(GameException::IndexOutOfBounds,
                                QString("Index %1 should be within [0,%2)").arg(i).arg(m_size));
        }
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

    GameObjectPtrList copy(length());
    copy.m_size = copy.m_capacity;

    int index = 0;
    for (const GameObjectPtr &item : *this) {
        copy.m_items[index] = item.copyUnresolved();
        index++;
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
            objects.append(object.cast<GameObject *>());
            objectNames.append(object->name());
            objectCounts.append(1);
        }
    }

    QStringList strings;
    for (int i = 0; i < objects.size(); i++) {
        const GameObject *object = objects[i];

        if (objectCounts[i] > 1) {
            if (i == 0 && options & Capitalized) {
                strings.append(Util::capitalize(Util::writtenNumber(objectCounts[i])) +
                               " " + object->plural());
            } else {
                strings.append(Util::writtenNumber(objectCounts[i]) + " " +
                               object->plural());
            }
        } else {
            if (object->indefiniteArticle().isEmpty()) {
                strings.append(object->name());
            } else if (options & DefiniteArticles) {
                strings.append((i == 0 && options & Capitalized ?
                                "The " : "the ") + object->name());
            } else {
                strings.append(object->indefiniteName(i == 0 ?
                                                      (options & Capitalized) :
                                                      NoOptions));
            }
        }
    }

    return Util::joinFancy(strings);
}

QString GameObjectPtrList::toUserString(const GameObjectPtrList &pointerList) {

    QStringList stringList;
    for (const GameObjectPtr &pointer : pointerList) {
        stringList.append(pointer.toString());
    }
    return "[ " + stringList.join(", ") + " ]";
}

void GameObjectPtrList::fromUserString(const QString &string, GameObjectPtrList &pointerList) {

    QStringList stringList = string.mid(1, string.length() - 2).split(',');
    pointerList.reserve(stringList.size());
    for (QString substring : stringList) {
        substring = substring.trimmed();
        if (!substring.isEmpty()) {
            GameObjectPtr pointer;
            GameObjectPtr::fromUserString(substring, pointer);
            pointerList.append(pointer);
        }
    }
}

QString GameObjectPtrList::toJsonString(const GameObjectPtrList &pointerList, Options options) {

    Q_UNUSED(options);

    QStringList stringList;
    for (const GameObjectPtr &pointer : pointerList) {
        stringList.append(GameObjectPtr::toJsonString(pointer));
    }
    return stringList.isEmpty() ? QString() : "[ " + stringList.join(", ") + " ]";
}

void GameObjectPtrList::fromVariant(const QVariant &variant, GameObjectPtrList &pointerList) {

    QList<QVariant> variantList = variant.toList();
    pointerList.reserve(variantList.size());
    for (const QVariant &item : variantList) {
        GameObjectPtr pointer;
        GameObjectPtr::fromVariant(item, pointer);
        pointerList.append(pointer);
    }
}
