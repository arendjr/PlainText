#ifndef COMBATMESSAGE_H
#define COMBATMESSAGE_H

#include <QMetaType>
#include <QScriptValue>
#include <QString>
#include <QVariantList>


class CombatMessage {

    public:
        CombatMessage();

        QString attackerMessage;
        QString defendantMessage;
        QString observerMessage;

        CombatMessage &operator=(const CombatMessage &other);
        bool operator==(const CombatMessage &other) const;
        bool operator!=(const CombatMessage &other) const;

        QString toString() const;

        static CombatMessage fromVariantList(const QVariantList &variantList);

        static QScriptValue toScriptValue(QScriptEngine *engine, const CombatMessage &message);
        static void fromScriptValue(const QScriptValue &object, CombatMessage &message);
};

typedef QList<CombatMessage> CombatMessageList;

Q_DECLARE_METATYPE(CombatMessage)
Q_DECLARE_METATYPE(CombatMessageList)

#endif // COMBATMESSAGE_H
