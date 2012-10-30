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


#define REGISTER_META_TYPE(x) \
    qRegisterMetaType<x>(); \
    qScriptRegisterMetaType(engine, x::toScriptValue, x::fromScriptValue);

#define REGISTER_META_TYPE_POINTER(x) \
    qRegisterMetaType<x *>(); \
    qScriptRegisterMetaType(engine, x::toScriptValue, x::fromScriptValue);

#define REGISTER_META_LIST_TYPE(x) \
    qRegisterMetaType<x>(); \
    qScriptRegisterSequenceMetaType<x>(engine);


void MetaTypeRegistry::registerMetaTypes(QScriptEngine *engine) {

    REGISTER_META_TYPE(CharacterStats)
    REGISTER_META_LIST_TYPE(CharacterStatsList)

    REGISTER_META_TYPE(Effect)
    REGISTER_META_LIST_TYPE(EffectList)

    REGISTER_META_TYPE_POINTER(GameObject)
    REGISTER_META_TYPE(GameObjectPtr)
    REGISTER_META_LIST_TYPE(GameObjectPtrList)

    REGISTER_META_TYPE(Modifier)
    REGISTER_META_LIST_TYPE(ModifierList)

    REGISTER_META_TYPE(Point3D)

    REGISTER_META_TYPE(ScriptFunction)
    REGISTER_META_TYPE(ScriptFunctionMap)

    REGISTER_META_TYPE(Vector3D)
}
