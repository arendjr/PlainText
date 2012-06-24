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

QScriptValue ScriptEngine::uncaughtException() const {

    return m_jsEngine.uncaughtException();
}

QScriptValue ScriptEngine::executeFunction(ScriptFunction &function, const GameObjectPtr &thisObject,
                                           const QVariantList &variantList) {

    QScriptValueList arguments;
    foreach (const QVariant &variant, variantList) {
        switch (variant.type()) {
            case QVariant::Int:
                arguments << m_jsEngine.toScriptValue(variant.toInt());
                break;
            case QVariant::String:
                arguments << m_jsEngine.toScriptValue(variant.toString());
                break;
            case QVariant::UserType:
                if (variant.userType() == QMetaType::type("GameObjectPtr")) {
                    arguments << m_jsEngine.toScriptValue(variant.value<GameObjectPtr>());
                } else if (variant.userType() == QMetaType::type("GameObjectPtrList")) {
                    arguments << m_jsEngine.toScriptValue(variant.value<GameObjectPtrList>());
                }
                break;
            default:
                qDebug() << "ScriptEngine::executeFunction(): Unknown argument type:" << variant.type();
        }
    }
    return function.value.call(m_jsEngine.toScriptValue(thisObject), arguments);
}

void ScriptEngine::setGlobalObject(const char *name, QObject *object) {

    m_jsEngine.globalObject().setProperty(name, m_jsEngine.newQObject(object,
                                                                      QScriptEngine::QtOwnership,
                                                                      QScriptEngine::ExcludeSuperClassContents |
                                                                      QScriptEngine::ExcludeDeleteLater));
}

void ScriptEngine::unsetGlobalObject(const char *name) {

    m_jsEngine.globalObject().setProperty(name, QScriptValue());
}

ScriptEngine::ScriptEngine() :
    QObject(),
    m_initialized(false) {

    s_instance = this;

    qScriptRegisterMetaType(&m_jsEngine, CharacterStats::toScriptValue, CharacterStats::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, Effect::toScriptValue, Effect::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, GameObject::toScriptValue, GameObject::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, GameObjectPtr::toScriptValue, GameObjectPtr::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, Modifier::toScriptValue, Modifier::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, ScriptFunction::toScriptValue, ScriptFunction::fromScriptValue);
    qScriptRegisterMetaType(&m_jsEngine, ScriptFunctionMap::toScriptValue, ScriptFunctionMap::fromScriptValue);
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

    m_initialized = true;
}

ScriptEngine::~ScriptEngine() {
}
