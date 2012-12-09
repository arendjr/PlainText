#include "statsitem.h"

#include <QDateTime>

#include "realm.h"


#define super Item

StatsItem::StatsItem(Realm *realm, GameObjectType objectType, uint id, Options options) :
    super(realm, objectType, id, options),
    m_stats(0),
    m_modifiersTimerId(0) {
}

StatsItem::~StatsItem() {
}

void StatsItem::setStats(const CharacterStats &stats) {

    if (m_stats != stats) {
        m_stats = stats;

        setModified();

        changeStats(m_stats);
    }
}

void StatsItem::addModifier(const Modifier &modifier) {

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    int nextTimeout = updateModifiers(now);

    if (nextTimeout == -1 || modifier.duration < nextTimeout) {
        if (m_modifiersTimerId) {
            realm()->stopTimer(m_modifiersTimerId);
        }
        m_modifiersTimerId = realm()->startTimer(this, modifier.duration);
    }

    m_modifiers.append(modifier);
    m_modifiers.last().started = now;
}

void StatsItem::clearModifiers() {

    m_modifiers.clear();
    if (m_modifiersTimerId) {
        realm()->stopTimer(m_modifiersTimerId);
        m_modifiersTimerId = 0;
    }
}

void StatsItem::clearNegativeModifiers() {

    for (int i = 0; i < m_modifiers.length(); i++) {
        Modifier &modifier = m_modifiers[i];
        CharacterStats &stats = modifier.stats;

        if (stats.total() < 0)  {
            m_modifiers.removeAt(i);
            i--;
            continue;
        }
    }

    if (m_modifiers.length() == 0) {
        clearModifiers();
    }
}

void StatsItem::invokeTimer(int timerId) {

    if (timerId == m_modifiersTimerId) {
        int nextTimeout = updateModifiers(QDateTime::currentMSecsSinceEpoch());
        m_modifiersTimerId = (nextTimeout > -1 ? realm()->startTimer(this, nextTimeout) : 0);
    } else {
        super::invokeTimer(timerId);
    }
}

void StatsItem::killAllTimers() {

    super::killAllTimers();

    clearModifiers();
}

void StatsItem::changeStats(const CharacterStats &newStats) {

    Q_UNUSED(newStats);
}

int StatsItem::updateModifiers(qint64 now) {

    int nextTimeout = -1;
    for (int i = 0; i < m_modifiers.length(); i++) {
        Modifier &modifier = m_modifiers[i];

        int msecsLeft = (modifier.started + modifier.duration) - now;
        if (msecsLeft > 0) {
            if (nextTimeout == -1 || msecsLeft < nextTimeout) {
                nextTimeout = msecsLeft;
            }
        } else {
            m_modifiers.removeAt(i);
            i--;
        }
    }

    return nextTimeout;
}
