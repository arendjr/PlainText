#include "scriptengine.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMetaType>

#include "diskutil.h"
#include "gameobject.h"
#include "metatyperegistry.h"
#include "scriptwindow.h"


static ScriptEngine *s_instance = nullptr;


ScriptEngine::ScriptEngine() :
    QObject() {

    s_instance = this;

    MetaTypeRegistry::registerMetaTypes(&m_jsEngine);

    ScriptWindow *window = new ScriptWindow(m_jsEngine.globalObject(), this);
    m_jsEngine.setGlobalObject(window->toScriptValue());
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
    
    QDir commandsDir(DiskUtil::dataDir() + "/commands");
    for (const QString &entry : commandsDir.entryList(QDir::Files)) {
        loadScript(commandsDir.path() + "/" + entry);
    }
    QDir adminCommandsDir(DiskUtil::dataDir() + "/commands/admin");
    for (const QString &entry : adminCommandsDir.entryList(QDir::Files)) {
        loadScript(adminCommandsDir.path() + "/" + entry);
    }

    loadScript(DiskUtil::dataDir() + "/scripts/sessionhandler.js");
}

void ScriptEngine::loadScript(const QString &path) {

    QFile file(path);
    QFileInfo info(path);
    if (file.open(QIODevice::ReadOnly)) {
        evaluate(file.readAll(), "commands/command.js");
        file.close();
        if (hasUncaughtException()) {
            qWarning() << "Exception while evaluating " << info.fileName() << ": "
                       << uncaughtException().toString();
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
    m_jsEngine.evaluate("");
    return exception;
}

QScriptValue ScriptEngine::executeFunction(ScriptFunction &function,
                                           const GameObjectPtr &thisObject,
                                           const QScriptValueList &arguments) {

    QScriptValue result = function.value.call(m_jsEngine.toScriptValue(thisObject), arguments);
    if (hasUncaughtException()) {
        QScriptValue exception = uncaughtException();
        qWarning() << "Script Exception: " << exception.toString().toUtf8().constData() << endl
                   << "While executing function: " << function.source.toUtf8().constData();
    }
    return result;
}

QScriptValue ScriptEngine::toScriptValue(GameObject *object) {

    return m_jsEngine.toScriptValue(object);
}

QScriptValue ScriptEngine::toScriptValue(const GameObjectPtr &object) {

    return m_jsEngine.toScriptValue(object);
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
