#include "metatyperegistry.h"

#include <QMetaType>
#include <QScriptEngine>

#include "characterstats.h"
#include "effect.h"
#include "gameobject.h"
#include "gameobjectptr.h"
#include "modifier.h"
#include "point3d.h"
#include "scriptfunctionmap.h"
#include "vector3d.h"


#define REGISTER_META_TYPE(x) {                                                                   \
    qRegisterMetaType<x>();                                                                       \
    qScriptRegisterMetaType(engine, x::toScriptValue, x::fromScriptValue);                        \
}

#define REGISTER_META_TYPE_POINTER(x) {                                                           \
    qRegisterMetaType<x *>();                                                                     \
    qScriptRegisterMetaType(engine, x::toScriptValue, x::fromScriptValue);                        \
}

#define REGISTER_META_LIST_TYPE(x) {                                                              \
    qRegisterMetaType<x>();                                                                       \
    qScriptRegisterSequenceMetaType<x>(engine);                                                   \
}

#define REGISTER_SERIALIZABLE(x) {                                                                \
    UserStringConverters userStringConverters;                                                    \
    userStringConverters.typeToUserStringConverter = convert##x##ToUserString;                    \
    userStringConverters.userStringToTypeConverter = convertUserStringTo##x;                      \
    s_userStringConvertersMap.insert(#x, userStringConverters);                                   \
    JsonConverters jsonConverters;                                                                \
    jsonConverters.typeToJsonStringConverter = convert##x##ToJsonString;                          \
    jsonConverters.jsonVariantToTypeConverter = convertJsonVariantTo##x;                          \
    s_jsonConvertersMap.insert(#x, jsonConverters);                                               \
}

#define REGISTER_SERIALIZABLE_META_TYPE(x) {                                                      \
    qRegisterMetaType<x>();                                                                       \
    qScriptRegisterMetaType(engine, x::toScriptValue, x::fromScriptValue);                        \
    REGISTER_SERIALIZABLE(x)                                                                      \
}

#define REGISTER_SERIALIZABLE_META_LIST_TYPE(x) {                                                 \
    qRegisterMetaType<x>();                                                                       \
    qScriptRegisterSequenceMetaType<x>(engine);                                                   \
    REGISTER_SERIALIZABLE(x)                                                                      \
}


QMap<QString, MetaTypeRegistry::UserStringConverters> MetaTypeRegistry::s_userStringConvertersMap;
QMap<QString, MetaTypeRegistry::JsonConverters> MetaTypeRegistry::s_jsonConvertersMap;


void MetaTypeRegistry::registerMetaTypes(QScriptEngine *engine) {

    REGISTER_SERIALIZABLE_META_TYPE(CharacterStats)

    REGISTER_META_TYPE(Effect)
    REGISTER_META_LIST_TYPE(EffectList)

    REGISTER_META_TYPE_POINTER(GameObject)
    REGISTER_SERIALIZABLE_META_TYPE(GameObjectType)
    REGISTER_SERIALIZABLE_META_TYPE(GameObjectPtr)
    REGISTER_SERIALIZABLE_META_LIST_TYPE(GameObjectPtrList)

    REGISTER_META_TYPE(Modifier)
    REGISTER_META_LIST_TYPE(ModifierList)

    REGISTER_SERIALIZABLE_META_TYPE(Point3D)

    REGISTER_SERIALIZABLE_META_TYPE(ScriptFunction)
    REGISTER_SERIALIZABLE_META_TYPE(ScriptFunctionMap)

    REGISTER_SERIALIZABLE_META_TYPE(Vector3D)
}

MetaTypeRegistry::UserStringConverters MetaTypeRegistry::userStringConverters(const char *type) {

    if (type) {
        QString key(type);
        if (s_userStringConvertersMap.contains(key)) {
            return s_userStringConvertersMap[key];
        }
    }

    UserStringConverters converters;
    converters.typeToUserStringConverter = nullptr;
    converters.userStringToTypeConverter = nullptr;
    return converters;
}

MetaTypeRegistry::JsonConverters MetaTypeRegistry::jsonConverters(const char *type) {

    if (type) {
        QString key(type);
        if (s_jsonConvertersMap.contains(key)) {
            return s_jsonConvertersMap[key];
        }
    }

    JsonConverters converters;
    converters.typeToJsonStringConverter = nullptr;
    converters.jsonVariantToTypeConverter = nullptr;
    return converters;
}
