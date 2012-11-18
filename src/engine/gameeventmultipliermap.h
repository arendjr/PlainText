#ifndef GAMEEVENTMULTIPLIERMAP_H
#define GAMEEVENTMULTIPLIERMAP_H

#include <QList>

#include "gameevent.h"
#include "metatyperegistry.h"


class GameEventMultiplierMap {

    public:
        GameEventMultiplierMap();
        GameEventMultiplierMap(const GameEventMultiplierMap &other);

        void clear();

        QList<GameEventType> keys() const;

        bool operator!=(const GameEventMultiplierMap &other) const;
        GameEventMultiplierMap &operator=(const GameEventMultiplierMap &other);
        bool operator==(const GameEventMultiplierMap &other) const;

        double &operator[](const GameEventType &key);
        double operator[](const GameEventType &key) const;

        static QString toUserString(const GameEventMultiplierMap &multipliers);
        static void fromUserString(const QString &string, GameEventMultiplierMap &multipliers);

        static QString toJsonString(const GameEventMultiplierMap &multipliers,
                                    Options options = NoOptions);
        static void fromVariant(const QVariant &variant, GameEventMultiplierMap &multipliers);

        static QScriptValue toScriptValue(QScriptEngine *engine,
                                          const GameEventMultiplierMap &multipliers);
        static void fromScriptValue(const QScriptValue &object,
                                    GameEventMultiplierMap &multipliers);

    private:
        double m_multipliers[GameEventType::NumValues];
};

PT_DECLARE_SERIALIZABLE_METATYPE(GameEventMultiplierMap)

#endif // GAMEEVENTMULTIPLIERMAP_H
