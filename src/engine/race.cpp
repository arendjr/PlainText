#include "race.h"


Race::Race(Realm *realm, uint id, Options options) :
    GameObject(realm, "race", id, options),
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

void Race::setStartingArea(const GameObjectPtr &startingArea) {

    if (m_startingArea != startingArea) {
        m_startingArea = startingArea;

        setModified();
    }
}

void Race::setHitMessages(const CombatMessageList &hitMessages) {

    if (m_hitMessages != hitMessages) {
        m_hitMessages = hitMessages;

        setModified();
    }
}

void Race::setMissMessages(const CombatMessageList &missMessages) {

    if (m_missMessages != missMessages) {
        m_missMessages = missMessages;

        setModified();
    }
}

void Race::setPlayerSelectable(bool playerSelectable) {

    if (m_playerSelectable != playerSelectable) {
        m_playerSelectable = playerSelectable;

        setModified();
    }
}
