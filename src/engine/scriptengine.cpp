#include "scriptengine.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMetaType>

#include "diskutil.h"
#include "gameobject.h"
#include "metatyperegistry.h"


static ScriptEngine *s_instance = nullptr;


ScriptEngine::ScriptEngine() :
    QObject() {

    s_instance = this;

    MetaTypeRegistry::registerMetaTypes(&m_jsEngine);
}

ScriptEngine::~ScriptEngine() {
}

ScriptEngine *ScriptEngine::instance() {

    return s_instance;
}

void ScriptEngine::loadScripts() {

    loadScript(":/script/util.js");
    loadScript(":/script/commands/command.js");
    loadScript(":/script/commands/admin/admincommand.js");

    loadScripts(DiskUtil::dataDir() + "/commands");
    loadScripts(DiskUtil::dataDir() + "/scripts");
}

void ScriptEngine::loadScripts(const QString &dirPath) {

    QDir dir(dirPath);
    for (const QString &entry : dir.entryList(QDir::Files)) {
        loadScript(dirPath + "/" + entry);
    }
    for (const QString &entry : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        loadScripts(dirPath + "/" + entry);
    }
}

void ScriptEngine::loadScript(const QString &path) {

    QFile file(path);
    QFileInfo info(path);
    if (file.open(QIODevice::ReadOnly)) {
        evaluate(file.readAll(), "commands/command.js");
        file.close();
        if (hasUncaughtException()) {
            QScriptValue exception = uncaughtException();
            qWarning() << "Exception while evaluating " << info.fileName() << ": "
                       << exception.toString().toUtf8().constData() << endl
                       << "Backtrace:" << endl
                       << exception.property("backtrace").toString().toUtf8().constData();
        }
    } else {
        qWarning() << "Could not open " << info.fileName();
    }
}

QScriptValue ScriptEngine::evaluate(const QString &program,
                                    const QString &fileName, int lineNumber) {

    return m_jsEngine.evaluate(program, fileName, lineNumber);
}

ScriptFunction ScriptEngine::defineFunction(const QString &program,
                                            const QString &fileName, int lineNumber) {

    QString source = program;
    if (source.startsWith("function")) {
        source = "(" + source + ")";
    }

    ScriptFunction function;
    function.value = m_jsEngine.evaluate(source, fileName, lineNumber);
    function.source = program;
    return function;
}

bool ScriptEngine::hasUncaughtException() const {

    return m_jsEngine.hasUncaughtException();
}

QScriptValue ScriptEngine::uncaughtException() {

    QScriptValue exception = m_jsEngine.uncaughtException();
    QStringList backtrace = m_jsEngine.uncaughtExceptionBacktrace();
    m_jsEngine.evaluate("");
    exception.setProperty("backtrace", QString("  " + backtrace.join("\n  ")));
    return exception;
}

QScriptValue ScriptEngine::executeFunction(ScriptFunction &function,
                                           const GameObjectPtr &thisObject,
                                           const QScriptValueList &arguments) {

    QScriptValue result = function.value.call(m_jsEngine.toScriptValue(thisObject), arguments);
    if (hasUncaughtException()) {
        QScriptValue exception = uncaughtException();
        qWarning() << "Script Exception: " << exception.toString().toUtf8().constData() << endl
                   << "While executing function: " << function.source.toUtf8().constData()
                   << "Backtrace:" << endl
                   << exception.property("backtrace").toString().toUtf8().constData();
    }
    return result;
}

QScriptValue ScriptEngine::toScriptValue(GameObject *object) {

    return GameObject::toScriptValue(&m_jsEngine, object);
}

QScriptValue ScriptEngine::toScriptValue(const GameObjectPtr &object) {

    return GameObjectPtr::toScriptValue(&m_jsEngine, object);
}

void ScriptEngine::setGlobalObject(const char *name, QObject *object) {

    m_jsEngine.globalObject()
              .setProperty(name, m_jsEngine.newQObject(object,
                                                       QScriptEngine::QtOwnership,
                                                       QScriptEngine::ExcludeDeleteLater));
}

void ScriptEngine::unsetGlobalObject(const char *name) {

    m_jsEngine.globalObject().setProperty(name, QScriptValue());
}
