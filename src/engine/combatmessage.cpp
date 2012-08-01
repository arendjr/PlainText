#include "combatmessage.h"

#include "conversionutil.h"
#include "gameexception.h"

#include <QScriptEngine>


CombatMessage::CombatMessage() {
}

CombatMessage &CombatMessage::operator=(const CombatMessage &other) {

    attackerMessage = other.attackerMessage;
    defendantMessage = other.defendantMessage;
    observerMessage = other.observerMessage;
    return *this;
}

bool CombatMessage::operator==(const CombatMessage &other) const {

    return (attackerMessage == other.attackerMessage &&
            defendantMessage == other.defendantMessage &&
            observerMessage == other.observerMessage);
}

bool CombatMessage::operator!=(const CombatMessage &other) const {

    return (attackerMessage != other.attackerMessage ||
            defendantMessage != other.defendantMessage ||
            observerMessage != other.observerMessage);
}

QString CombatMessage::toString() const {

    return QString("[ %1, %2, %3 ]")
           .arg(ConversionUtil::jsString(attackerMessage),
                ConversionUtil::jsString(defendantMessage),
                ConversionUtil::jsString(observerMessage));
}

CombatMessage CombatMessage::fromVariantList(const QVariantList &variantList) {

    if (variantList.length() != 3) {
        throw GameException(GameException::InvalidCombatMessage);
    }

    CombatMessage message;
    message.attackerMessage = variantList[0].toString();
    message.defendantMessage = variantList[1].toString();
    message.observerMessage = variantList[2].toString();
    return message;
}

QScriptValue CombatMessage::toScriptValue(QScriptEngine *engine, const CombatMessage &message) {

    QScriptValue array = engine->newArray(3);
    array.setProperty(0, message.attackerMessage);
    array.setProperty(1, message.defendantMessage);
    array.setProperty(2, message.observerMessage);
    return array;
}

void CombatMessage::fromScriptValue(const QScriptValue &object, CombatMessage &message) {

    if (!object.isArray()) {
        throw GameException(GameException::InvalidCombatMessage);
    }

    message.attackerMessage = object.property(0).toString();
    message.defendantMessage = object.property(1).toString();
    message.observerMessage = object.property(2).toString();
}
