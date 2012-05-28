#include "race.h"


Race::Race(uint id, Options options) :
    GameObject("race", id, options) {

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
