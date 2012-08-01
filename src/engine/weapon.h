#ifndef WEAPON_H
#define WEAPON_H

#include "characterstats.h"
#include "combatmessage.h"
#include "statsitem.h"


class Weapon : public StatsItem {

    Q_OBJECT

    public:
        Weapon(Realm *realm, uint id, Options options = NoOptions);
        virtual ~Weapon();

        const CombatMessageList &hitMessages() const { return m_hitMessages; }
        void setHitMessages(const CombatMessageList &hitMessages);
        Q_PROPERTY(CombatMessageList hitMessages READ hitMessages WRITE setHitMessages)

        const CombatMessageList &missMessages() const { return m_missMessages; }
        void setMissMessages(const CombatMessageList &missMessages);
        Q_PROPERTY(CombatMessageList missMessages READ missMessages WRITE setMissMessages)

    private:
        CombatMessageList m_hitMessages;
        CombatMessageList m_missMessages;
};

#endif // WEAPON_H
