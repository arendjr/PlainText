#include "listmethodscommand.h"

#include <QScriptValueIterator>

#include "engine/scriptengine.h"
#include "engine/util.h"


ListMethodsCommand::ListMethodsCommand(Player *character, QObject *parent) :
    AdminCommand(character, parent) {

    setDescription("List all JavaScript-accessible methods of an object.\n"
                   "\n"
                   "Usage: list-methods <object-name> [#]");
}

ListMethodsCommand::~ListMethodsCommand() {
}

void ListMethodsCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();

    GameObjectPtrList objects = takeObjects(currentArea()->objects());
    if (!requireUnique(objects, "Object not found.", "Object is not unique.")) {
        return;
    }

    GameObjectPtr object = objects[0];
    send(QString("These are all known methods of %1:\n"
                 "\n").arg(Util::highlight(QString("object #%1").arg(object->id()))));

    ScriptEngine *scriptEngine = ScriptEngine::instance();
    QScriptValue value = scriptEngine->evaluate(QString("Object.keys($('%1:%2'))")
                                                .arg(object->objectType()).arg(object->id()));

    QScriptValueIterator it(value);
    while (it.hasNext()) {
        it.next();
        QString key = it.value().toString();
        if (key.contains('(') && !key.startsWith("destroyed(") &&
            !key.contains(QRegExp("[(,]Color\\)")) && !key.contains(QRegExp("[(,]Options\\)"))) {
            key = key.replace("QString", "string").replace("QScriptValue", "value")
                     .replace("GameObjectPtr", "GameObject").replace(",", ", ");
            send(QString("  %1\n").arg(Util::highlight(key)));
        }
    }
}
