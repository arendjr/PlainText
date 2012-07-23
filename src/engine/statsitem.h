#ifndef STATSITEM_H
#define STATSITEM_H

#include "characterstats.h"
#include "item.h"
#include "modifier.h"


class StatsItem : public Item {

    Q_OBJECT

    public:
        StatsItem(Realm *realm, const char *objectType, uint id, Options options = NoOptions);
        virtual ~StatsItem();

        const CharacterStats &stats() const { return m_stats; }
        virtual void setStats(const CharacterStats &stats);
        Q_PROPERTY(CharacterStats stats READ stats WRITE setStats)

        Q_INVOKABLE virtual CharacterStats totalStats() const;

        ModifierList modifiers() const { return m_modifiers; }
        Q_INVOKABLE void addModifier(const Modifier &modifier);
        Q_INVOKABLE void clearModifiers();
        Q_INVOKABLE void clearNegativeModifiers();
        Q_PROPERTY(ModifierList modifiers READ modifiers STORED false)

        virtual void invokeTimer(int timerId);

        virtual void killAllTimers();

    private:
        CharacterStats m_stats;

        ModifierList m_modifiers;
        int m_modifiersTimerId;

        int updateModifiers(qint64 now);
};

#endif // STATSITEM_H
