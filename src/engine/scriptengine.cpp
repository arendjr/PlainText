#include "scriptengine.h"

#include <QDebug>
#include <QFile>
#include <QMetaType>

#include "characterstats.h"
#include "effect.h"
#include "gameobject.h"
#include "modifier.h"
#include "scriptfunctionmap.h"
#include "scriptwindow.h"


static ScriptEngine *s_instance = nullptr;


ScriptEngine::ScriptEngine() :
    QObject() {

    s_instance = this;

    qScriptRegisterMetaType(&m_jsEngine, CharacterStats::toScriptValue,
                                         CharacterStats::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, Effect::toScriptValue, Effect::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, GameObject::toScriptValue, GameObject::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, GameObjectPtr::toScriptValue,
                                         GameObjectPtr::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, Modifier::toScriptValue, Modifier::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, ScriptFunction::toScriptValue,
                                         ScriptFunction::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, ScriptFunctionMap::toScriptValue,
                                         ScriptFunctionMap::fromScriptValue);
    qScriptRegisterSequenceMetaType<CharacterStatsList>(&m_jsEngine);
    qScriptRegisterSequenceMetaType<EffectList>(&m_jsEngine);
    qScriptRegisterSequenceMetaType<GameObjectPtrList>(&m_jsEngine);
    qScriptRegisterSequenceMetaType<ModifierList>(&m_jsEngine);

    ScriptWindow *window = new ScriptWindow(m_jsEngine.globalObject(), this);
    m_jsEngine.setGlobalObject(window->toScriptValue());

    QFile utilJs(":/script/util.js");
    if (utilJs.open(QIODevice::ReadOnly)) {
        m_jsEngine.evaluate(utilJs.readAll(), "util.js");
        utilJs.close();
    } else {
        qWarning() << "Could not open util.js.";
    }
}

ScriptEngine::~ScriptEngine() {
}

ScriptEngine *ScriptEngine::instance() {

    return s_instance;
}

QScriptValue ScriptEngine::evaluate(const QString &program,
                                    const QString &fileName, int lineNumber) {

    return m_jsEngine.evaluate(program, fileName, lineNumber);
}

ScriptFunction ScriptEngine::defineFunction(const QString &program,
                                            const QString &fileName, int lineNumber) {

    ScriptFunction function;
    function.value = m_jsEngine.evaluate(program, fileName, lineNumber);
    function.source = program;
    return function;
}

bool ScriptEngine::hasUncaughtException() const {

    return m_jsEngine.hasUncaughtException();
}

QScriptValue ScriptEngine::uncaughtException() const {

    return m_jsEngine.uncaughtException();
}

QScriptValue ScriptEngine::executeFunction(ScriptFunction &function,
                                           const GameObjectPtr &thisObject,
                                           const QScriptValueList &arguments) {

    QScriptValue result =  function.value.call(m_jsEngine.toScriptValue(thisObject), arguments);
    if (m_jsEngine.hasUncaughtException()) {
        QScriptValue exception = m_jsEngine.uncaughtException();
        qWarning() << "Script Exception: " << exception.toString().toUtf8().constData() << endl
                   << "While executing function: " << function.source.toUtf8().constData();
        m_jsEngine.evaluate("");
    }
    return result;
}

QScriptValue ScriptEngine::toScriptValue(GameObject *object) {

    return m_jsEngine.toScriptValue(object);
}

QScriptValue ScriptEngine::toScriptValue(const GameObjectPtr &object) {

    return m_jsEngine.toScriptValue(object);
}

QScriptValue ScriptEngine::toScriptValue(const GameObjectPtrList &list) {

    return m_jsEngine.toScriptValue(list);
}

void ScriptEngine::setGlobalObject(const char *name, QObject *object) {

    m_jsEngine.globalObject()
              .setProperty(name, m_jsEngine.newQObject(object,
                                                       QScriptEngine::QtOwnership,
                                                       QScriptEngine::ExcludeSuperClassContents |
                                                       QScriptEngine::ExcludeDeleteLater));
}

void ScriptEngine::unsetGlobalObject(const char *name) {

    m_jsEngine.globalObject().setProperty(name, QScriptValue());
}
