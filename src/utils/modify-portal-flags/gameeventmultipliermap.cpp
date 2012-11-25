#include "gameeventmultipliermap.h"

#include <QScriptValueIterator>

#include "gameexception.h"


GameEventMultiplierMap::GameEventMultiplierMap() {

    clear();
}

GameEventMultiplierMap::GameEventMultiplierMap(const GameEventMultiplierMap &other) {

    for (int i = 0; i < (int) GameEventType::NumValues; i++) {
        m_multipliers[i] = other.m_multipliers[i];
    }
}

void GameEventMultiplierMap::clear() {

    for (int i = 0; i < (int) GameEventType::NumValues; i++) {
        m_multipliers[i] = 1.0;
    }
}

QList<GameEventType> GameEventMultiplierMap::keys() const {

    QList<GameEventType> keys;
    for (int i = 1; i < (int) GameEventType::NumValues; i++) {
        keys.append(GameEventType((GameEventType::Values) i));
    }
    return keys;
}

bool GameEventMultiplierMap::operator!=(const GameEventMultiplierMap &other) const {

    for (int i = 0; i < (int) GameEventType::NumValues; i++) {
        if (m_multipliers[i] != other.m_multipliers[i]) {
            return true;
        }
    }
    return false;
}

GameEventMultiplierMap &GameEventMultiplierMap::operator=(const GameEventMultiplierMap &other) {

    for (int i = 0; i < (int) GameEventType::NumValues; i++) {
        m_multipliers[i] = other.m_multipliers[i];
    }
    return *this;
}

bool GameEventMultiplierMap::operator==(const GameEventMultiplierMap &other) const {

    for (int i = 0; i < (int) GameEventType::NumValues; i++) {
        if (m_multipliers[i] != other.m_multipliers[i]) {
            return false;
        }
    }
    return true;
}

double &GameEventMultiplierMap::operator[](const GameEventType &key) {

    return m_multipliers[key.intValue()];
}

double GameEventMultiplierMap::operator[](const GameEventType &key) const {

    return m_multipliers[key.intValue()];
}

QString GameEventMultiplierMap::toUserString(const GameEventMultiplierMap &multipliers) {

    QStringList stringList;
    for (const GameEventType &eventType : multipliers.keys()) {
        stringList << QString("%1: %2").arg(eventType.toString())
                                       .arg(multipliers[eventType]);
    }
    return stringList.join("\n");
}

void GameEventMultiplierMap::fromUserString(const QString &string,
                                            GameEventMultiplierMap &multipliers) {

    Q_UNUSED(string)
    Q_UNUSED(multipliers)

    throw GameException(GameException::NotSupported,
                        "Converting user strings to game event multiplier map not (yet) supported");
}

QString GameEventMultiplierMap::toJsonString(const GameEventMultiplierMap &multipliers,
                                             Options options) {

    Q_UNUSED(options)

    QStringList stringList;
    for (const GameEventType &eventType : multipliers.keys()) {
        double value = multipliers[eventType];
        if (value != 1.0) {
            stringList << QString("%1: %2").arg(ConversionUtil::jsString(eventType.toString()))
                                           .arg(value);
        }
    }
    return stringList.isEmpty() ? QString() : "{ " + stringList.join(", ") + " }";
}

void GameEventMultiplierMap::fromVariant(const QVariant &variant,
                                         GameEventMultiplierMap &multipliers) {

    QVariantMap variantMap = variant.toMap();
    for (const QString &key : variantMap.keys()) {
        multipliers[GameEventType::fromString(key)] = variantMap[key].toDouble();
    }
}

QScriptValue GameEventMultiplierMap::toScriptValue(QScriptEngine *engine,
                                                   const GameEventMultiplierMap &multipliers) {

    QScriptValue value = engine->newObject();
    for (const GameEventType &key : multipliers.keys()) {
        value.setProperty(key.toString(), multipliers[key]);
    }
    return value;
}

void GameEventMultiplierMap::fromScriptValue(const QScriptValue &value,
                                             GameEventMultiplierMap &multipliers) {

    multipliers.clear();
    QScriptValueIterator it(value);
    while (it.hasNext()) {
        it.next();
        multipliers[GameEventType::fromString(it.name())] = it.value().toNumber();
    }
}
