#include "scriptengine.h"

#include "gameobjectptr.h"
#include "realm.h"


void ScriptEngine::instantiate() {

    Q_ASSERT(s_instance == 0);
    new ScriptEngine();
}

void ScriptEngine::destroy() {

    delete s_instance;
    s_instance = 0;
}

QScriptValue ScriptEngine::evaluate(const QString &program, const QString &fileName, int lineNumber) {

    QScriptValue value = m_jsEngine.evaluate(program, fileName, lineNumber);
    value.setData(program);
    return value;
}

bool ScriptEngine::hasUncaughtException() const {

    return m_jsEngine.hasUncaughtException();
}

bool ScriptEngine::executeTrigger(QScriptValue &trigger, GameObject *object, Area *area, Character *character) {

    QScriptValueList arguments;
    arguments << m_jsEngine.toScriptValue((GameObject *) area)
              << m_jsEngine.toScriptValue((GameObject *) character);
    return trigger.call(m_jsEngine.toScriptValue(object), arguments).toBool();
}

ScriptEngine::ScriptEngine() :
    QObject(),
    m_initialized(false) {

    s_instance = this;

    qScriptRegisterMetaType(&m_jsEngine, GameObject::toScriptValue, GameObject::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, GameObjectPtr::toScriptValue, GameObjectPtr::fromScriptValue);
    qScriptRegisterSequenceMetaType<GameObjectPtrList>(&m_jsEngine);

    m_jsEngine.globalObject().setProperty("Realm", m_jsEngine.newQObject(Realm::instance()));

    m_initialized = true;
}

ScriptEngine::~ScriptEngine() {
}
