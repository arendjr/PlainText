#include "race.h"


#define super GameObject

Race::Race(Realm *realm, uint id, Options options) :
    super(realm, "race", id, options),
    m_playerSelectable(false) {

    setAutoDelete(false);
}

Race::~Race() {
}

void Race::setAdjective(const QString &adjective) {

    if (m_adjective != adjective) {
        m_adjective = adjective;

        setModified();
    }
}

void Race::setStats(const CharacterStats &stats) {

    if (m_stats != stats) {
        m_stats = stats;

        setModified();
    }
}

void Race::setHeight(int height) {

    if (m_height != height) {
        m_height = height;

        setModified();
    }
}

void Race::setWeight(int weight) {

    if (m_weight != weight) {
        m_weight = weight;

        setModified();
    }
}

void Race::setClasses(const GameObjectPtrList &classes) {

    if (m_classes != classes) {
        m_classes = classes;

        setModified();
    }
}

void Race::setStartingRoom(const GameObjectPtr &startingRoom) {

    if (m_startingRoom != startingRoom) {
        m_startingRoom = startingRoom;

        setModified();
    }
}

void Race::setPlayerSelectable(bool playerSelectable) {

    if (m_playerSelectable != playerSelectable) {
        m_playerSelectable = playerSelectable;

        setModified();
    }
}
