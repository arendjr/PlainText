#include "scriptengine.h"

#include "realm.h"
#include "scriptfunctionmap.h"


ScriptEngine *ScriptEngine::s_instance = 0;


void ScriptEngine::instantiate() {

    Q_ASSERT(s_instance == 0);
    new ScriptEngine();
}

void ScriptEngine::destroy() {

    delete s_instance;
    s_instance = 0;
}

QScriptValue ScriptEngine::evaluate(const QString &program, const QString &fileName, int lineNumber) {

    return m_jsEngine.evaluate(program, fileName, lineNumber);
}

ScriptFunction ScriptEngine::defineFunction(const QString &program, const QString &fileName, int lineNumber) {

    ScriptFunction function;
    function.value = m_jsEngine.evaluate(program, fileName, lineNumber);
    function.source = program;
    return function;
}

bool ScriptEngine::hasUncaughtException() const {

    return m_jsEngine.hasUncaughtException();
}

bool ScriptEngine::executeFunction(ScriptFunction &function, const GameObjectPtr &thisObject,
                                   const GameObjectPtrList &objects) {

    QScriptValueList arguments;
    foreach (const GameObjectPtr &object, objects) {
        arguments << m_jsEngine.toScriptValue(object);
    }
    return function.value.call(m_jsEngine.toScriptValue(thisObject), arguments).toBool();
}

ScriptEngine::ScriptEngine() :
    QObject(),
    m_initialized(false) {

    s_instance = this;

    qScriptRegisterMetaType(&m_jsEngine, GameObject::toScriptValue, GameObject::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, GameObjectPtr::toScriptValue, GameObjectPtr::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, ScriptFunction::toScriptValue, ScriptFunction::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, ScriptFunctionMap::toScriptValue, ScriptFunctionMap::fromScriptValue);
    qScriptRegisterSequenceMetaType<GameObjectPtrList>(&m_jsEngine);

    m_jsEngine.globalObject().setProperty("Realm", m_jsEngine.newQObject(Realm::instance()));

    m_initialized = true;
}

ScriptEngine::~ScriptEngine() {
}
