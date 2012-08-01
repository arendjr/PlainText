#ifndef SHIELD_H
#define SHIELD_H

#include "characterstats.h"
#include "combatmessage.h"
#include "statsitem.h"


class Shield : public StatsItem {

    Q_OBJECT

    public:
        Shield(Realm *realm, uint id, Options options = NoOptions);
        virtual ~Shield();

        const CombatMessageList &deflectMessages() const { return m_deflectMessages; }
        void setDeflectMessages(const CombatMessageList &deflectMessages);
        Q_PROPERTY(CombatMessageList deflectMessages READ deflectMessages WRITE setDeflectMessages)

    private:
        CombatMessageList m_deflectMessages;
};

#endif // SHIELD_H
