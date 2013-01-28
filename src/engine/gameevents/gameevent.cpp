#include "gameevent.h"

#include <QHash>
#include <QScriptEngine>

#include "areaevent.h"
#include "floodevent.h"
#include "movementsoundevent.h"
#include "movementvisualevent.h"
#include "room.h"
#include "soundevent.h"
#include "speechevent.h"
#include "visualevent.h"


GameEvent::GameEvent(GameEventType eventType, Room *origin, double strength) :
    QObject(),
    m_eventType(eventType),
    m_origin(origin),
    m_nextVisitIndex(0) {

    addVisit(origin, strength);
}

GameEvent::~GameEvent() {
}

bool GameEvent::isSoundEvent() const {

    return m_eventType == GameEventType::Sound ||
           m_eventType == GameEventType::MovementSound ||
           m_eventType == GameEventType::Speech;
}

bool GameEvent::isVisualEvent() const {

    return m_eventType == GameEventType::Visual ||
           m_eventType == GameEventType::MovementVisual;
}

GameObjectPtr GameEvent::origin() const {

    return m_origin;
}

void GameEvent::setDescription(const QString &description) {

    m_description = description;
}

void GameEvent::setDistantDescription(const QString &distantDescription) {

    m_distantDescription = distantDescription;
}

void GameEvent::setVeryDistantDescription(const QString &veryDistantDescription) {

    m_veryDistantDescription = veryDistantDescription;
}

QString GameEvent::descriptionForStrengthAndCharacterInRoom(double strength, Character *character,
                                                            Room *room) const {

    Q_UNUSED(character)
    Q_UNUSED(room)

    if (strength >= 0.7 || m_distantDescription.isEmpty()) {
        return m_description;
    } else if (strength >= 0.4 || m_veryDistantDescription.isEmpty()) {
        return m_distantDescription;
    } else {
        return m_veryDistantDescription;
    }
}

void GameEvent::addExcludedCharacter(const GameObjectPtr &excludedCharacter) {

    m_excludedCharacters.append(excludedCharacter);
}

void GameEvent::setExcludedCharacters(const GameObjectPtrList &excludedCharacters) {

    m_excludedCharacters = excludedCharacters;
}

void GameEvent::addAffectedCharacter(const GameObjectPtr &affectedCharacter) {

    m_affectedCharacters.append(affectedCharacter);
}

void GameEvent::setAffectedCharacters(const GameObjectPtrList &affectedCharacters) {

    m_affectedCharacters = affectedCharacters;
}

void GameEvent::fire() {

    while (m_nextVisitIndex < m_visits.size()) {
        Visit visit = m_visits.value(m_nextVisitIndex);
        visitRoom(visit.room, visit.strength);

        m_nextVisitIndex++;
    }

    deleteLater();
}

int GameEvent::numVisitedRooms() const {

    return m_nextVisitIndex;
}

QScriptValue GameEvent::toScriptValue(QScriptEngine *engine, GameEvent *const &event) {

    return engine->newQObject(event, QScriptEngine::QtOwnership,
                              QScriptEngine::ExcludeDeleteLater |
                              QScriptEngine::PreferExistingWrapperObject);
}

void GameEvent::fromScriptValue(const QScriptValue &object, GameEvent *&event) {

    event = qobject_cast<GameEvent *>(object.toQObject());
    Q_ASSERT(event);
}

GameEvent *GameEvent::createByEventType(GameEventType eventType, Room *origin, double strength) {

    switch (eventType.value) {
        case GameEventType::Area:
            return new AreaEvent(origin, strength);
        case GameEventType::Flood:
            return new FloodEvent(origin, strength);
        case GameEventType::MovementSound:
            return new MovementSoundEvent(origin, strength);
        case GameEventType::MovementVisual:
            return new MovementVisualEvent(origin, strength);
        case GameEventType::Sound:
            return new SoundEvent(origin, strength);
        case GameEventType::Speech:
            return new SpeechEvent(origin, strength);
        case GameEventType::Visual:
            return new VisualEvent(origin, strength);
        case GameObjectType::Unknown:
        default:
            break;
    }

    throw GameException(GameException::UnknownGameEventType);
}

QVector<QMetaProperty> GameEvent::storedMetaProperties() const {

    static QHash<int, QVector<QMetaProperty> > storedProperties;
    if (!storedProperties.contains(m_eventType.intValue())) {
        QVector<QMetaProperty> properties;
        int count = metaObject()->propertyCount(),
            offset = GameEvent::staticMetaObject.propertyOffset();
        for (int i = offset; i < count; i++) {
            QMetaProperty metaProperty = metaObject()->property(i);
            if (metaProperty.isStored()) {
                properties << metaProperty;
            }
        }
        storedProperties[m_eventType.intValue()] = properties;
    }
    return storedProperties[m_eventType.intValue()];
}

void GameEvent::addVisit(Room *room, double strength) {

    int index = m_visitIndex.value(room);
    if (index == 0) {
        m_visits.append(Visit(room, strength));
        m_visitIndex[room] = m_visits.size();
    } else {
        index--;
        if (m_visits[index].strength < strength) {
            m_visits[index].strength = strength;
        }
    }
}

bool GameEvent::hasBeenVisited(Room *room) const {

    int index = m_visitIndex.value(room);
    return index > 0;
}

double GameEvent::strengthForRoom(Room *room) const {

    int index = m_visitIndex.value(room);
    if (index > 0) {
        return m_visits[index - 1].strength;
    } else {
        return 0.0;
    }
}
