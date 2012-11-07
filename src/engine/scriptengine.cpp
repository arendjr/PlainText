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
}

void ScriptEngine::loadScript(const QString &path) {

    QFile file(path);
    QFileInfo info(path);
    if (file.open(QIODevice::ReadOnly)) {
        m_jsEngine.evaluate(file.readAll(), "commands/command.js");
        file.close();
        if (m_jsEngine.hasUncaughtException()) {
            qWarning() << "Exception while evaluating " << info.fileName() << ": "
                       << m_jsEngine.uncaughtException().toString();
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

QScriptValue ScriptEngine::uncaughtException() const {

    return m_jsEngine.uncaughtException();
}

QScriptValue ScriptEngine::executeFunction(ScriptFunction &function,
                                           const GameObjectPtr &thisObject,
                                           const QScriptValueList &arguments) {

    QScriptValue result = function.value.call(m_jsEngine.toScriptValue(thisObject), arguments);
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

const QMap<QString, QString> &ScriptEngine::triggers() {

    static QMap<QString, QString> triggers;

    if (triggers.isEmpty()) {
        triggers.insert("onactive : void",
                        "The onactive trigger is invoked on any character when it's no longer "
                        "stunned, ie. when it can perform a new action again.");
        triggers.insert("onattack(attacker : character) : bool",
                        "The onattack trigger is invoked on any character when it's being "
                        "attacked.");
        triggers.insert("onbuy(buyer : character, boughtItem : optional item) : bool",
                        "The onbuy trigger is invoked on any character when something is being "
                        "bought from it. When boughtItem is omitted, the buyer is requesting an "
                        "overview of the things for sale.");
        triggers.insert("oncharacterattacked(attacker : character, defendant : character) : void",
                        "The oncharacterattacked trigger is invoked on any character in a room, "
                        "except for the attacker and defendant themselves, when another "
                        "character in that room emerges into combat.");
        triggers.insert("oncharacterdied(defendant : character, attacker : optional character) : "
                        "bool",
                        "The oncharacterdied trigger is invoked on any character in a room, when "
                        "another character in that room dies. When attacker is omitted, the "
                        "defendant died because of a non-combat cause (for example, poison).");
        triggers.insert("oncharacterentered(activator : character) : void",
                        "The oncharacterentered trigger is invoked on any character in a room when "
                        "another character enters that room.");
        triggers.insert("onclose(activator : character) : bool",
                        "The onclose trigger is invoked on any item or exit when it's closed.");
        triggers.insert("oncombat(attacker : character, defendant : character, observers : list, "
                        "damage : integer) : bool",
                        "This trigger is defined on the realm, and may be defined on individual "
                        "rooms. Its primary responsibility is generating the messages that are "
                        "displayed during combat, but it may also provide special combat effects. "
                        "If the room in which the combat takes place has this trigger defined, it "
                        "is used instead of the generic trigger from the realm. For this trigger, "
                        "the return value cannot be used to cancel the combat, but if a room's "
                        "trigger returns false it will fall back to the realm's trigger.");
        triggers.insert("ondie(attacker : optional character) : bool",
                        "The ondie trigger is invoked on any character when it dies. When "
                        "attacker is omitted, the character died because of a non-combat cause "
                        "(for example, poison).");
        triggers.insert("ondrink(activator : character) : bool",
                        "The ondrink trigger is invoked on any item when it's drunk.");
        triggers.insert("oneat(activator : character) : bool",
                        "The ondrink trigger is invoked on any item when it's eaten.");
        triggers.insert("onenter(activator : character) : bool",
                        "The onenter trigger is invoked on any exit when it's entered.");
        triggers.insert("onentered : void",
                        "The onentered trigger is invoked on any character when it entered a new "
                        "room.");
        triggers.insert("oncharacterexit(activator : character, exitName : string) : bool",
                        "The onexit trigger is invoked on any character in a room when another "
                        "character leaves that room.");
        triggers.insert("oninit : void",
                        "The oninit trigger is invoked once on every object when the game server "
                        "is started. Note: For characters that do have an onspawn trigger, but "
                        "no oninit trigger, onspawn is triggered instead.");
        triggers.insert("onopen(activator : character) : bool",
                        "The onopen trigger is invoked on any item or exit when it's opened.");
        triggers.insert("onreceive(giver : character, item : item or amount) : bool",
                        "The onreceive trigger is invoked on any character when something is "
                        "being given to it. Note that item may be a number instead of an item "
                        "object when an amount of gold is being given.");
        triggers.insert("onshout(activator : character, message : string) : void",
                        "The onshout trigger is invoked on any character when it hears someone "
                        "shout.");
        triggers.insert("onspawn : void",
                        "The onspawn trigger is invoked on any character when it respawns.");
        triggers.insert("ontalk(speaker : character, message : string) : void",
                        "The ontalk trigger is invoked on any character when talked to.");
        triggers.insert("onuse(activator : character) : void",
                        "The onuse trigger is invoked on any item when it's used.");
    }

    return triggers;
}
