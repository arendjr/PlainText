#ifndef STATSITEM_H
#define STATSITEM_H

#include "characterstats.h"
#include "item.h"
#include "modifier.h"


class StatsItem : public Item {

    Q_OBJECT

    public:
        StatsItem(Realm *realm, GameObjectType objectType, uint id, Options options = NoOptions);
        virtual ~StatsItem();

        const CharacterStats &stats() const { return m_stats; }
        void setStats(const CharacterStats &stats);
        Q_PROPERTY(CharacterStats stats READ stats WRITE setStats)

        ModifierList modifiers() const { return m_modifiers; }
        Q_INVOKABLE void addModifier(const Modifier &modifier);
        Q_INVOKABLE void clearModifiers();
        Q_INVOKABLE void clearNegativeModifiers();
        Q_PROPERTY(ModifierList modifiers READ modifiers STORED false)

        virtual void invokeTimer(int timerId);

        virtual void killAllTimers();

    protected:
        virtual void changeStats(const CharacterStats &newStats);

    private:
        CharacterStats m_stats;

        ModifierList m_modifiers;
        int m_modifiersTimerId;

        int updateModifiers(qint64 now);
};

#endif // STATSITEM_H
