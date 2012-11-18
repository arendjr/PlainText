#include "gameevent.h"

#include <QHash>
#include <QScriptEngine>

#include "room.h"


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

    return m_eventType == GameEventType::SoundEvent;
}

bool GameEvent::isVisualEvent() const {

    return m_eventType == GameEventType::VisualEvent;
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

void GameEvent::activate() {

    while (m_nextVisitIndex < m_visits.size()) {
        Visit visit = m_visits[m_nextVisitIndex];
        visitRoom(visit.room, visit.strength);

        m_nextVisitIndex++;
    }

    deleteLater();
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

    m_visits.append(Visit(room, strength));
}
