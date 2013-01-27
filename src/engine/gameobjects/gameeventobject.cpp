#include "gameeventobject.h"


#define super GameObject

GameEventObject::GameEventObject(Realm *realm, uint id, Options options) :
    super(realm, GameObjectType::Event, id, options) {
}

GameEventObject::~GameEventObject() {
}

void GameEventObject::setEventType(GameEventType eventType) {

    if (m_eventType != eventType) {
        m_eventType = eventType;

        setModified();
    }
}

void GameEventObject::setDescription(const QString &description) {

    if (m_description != description) {
        m_description = description;

        setModified();
    }
}

void GameEventObject::setDistantDescription(const QString &distantDescription) {

    if (m_distantDescription != distantDescription) {
        m_distantDescription = distantDescription;

        setModified();
    }
}

void GameEventObject::setVeryDistantDescription(const QString &veryDistantDescription) {

    if (m_veryDistantDescription != veryDistantDescription) {
        m_veryDistantDescription = veryDistantDescription;

        setModified();
    }
}

GameEvent *GameEventObject::createInRoom(Room *origin, double strength,
                                         const GameObjectPtrList &excludedCharacters) {

    GameEvent *event = GameEvent::createByEventType(m_eventType, origin, strength);

    for (const QMetaProperty &metaProperty : event->storedMetaProperties()) {
        const char *name = metaProperty.name();
        event->setProperty(name, property(name));
    }

    event->setExcludedCharacters(excludedCharacters);

    return event;
}
