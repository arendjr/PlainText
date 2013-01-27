#include "listmethodscommand.h"

#include <QScriptValueIterator>

#include "scriptengine.h"
#include "util.h"


#define super AdminCommand

ListMethodsCommand::ListMethodsCommand(QObject *parent) :
    super(parent) {

    setDescription("List all JavaScript-accessible methods of an object.\n"
                   "\n"
                   "Usage: list-methods <object-name> [#]");
}

ListMethodsCommand::~ListMethodsCommand() {
}

void ListMethodsCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = takeObject(currentRoom()->portals() + currentRoom()->characters() +
                                      currentRoom()->items());
    if (!requireSome(object, "Object not found.")) {
        return;
    }

    send(QString("These are all known methods of %1:\n"
                 "\n").arg(Util::highlight(QString("object #%1").arg(object->id()))));

    ScriptEngine *scriptEngine = ScriptEngine::instance();
    QScriptValue value = scriptEngine->evaluate(QString("(function(){"
                                                        "  var keys = [];"
                                                        "  var object = $('%1:%2');"
                                                        "  for (var key in object) {"
                                                        "    keys.push(key);"
                                                        "  }"
                                                        "  return keys;"
                                                        "})()")
                                                .arg(object->objectType().toString())
                                                .arg(object->id()));

    QScriptValueIterator it(value);
    while (it.hasNext()) {
        it.next();
        QString signature = it.value().toString();
        if (signature.contains('(') && !signature.startsWith("destroyed(") &&
            !signature.contains(QRegExp("[(,]Options\\)"))) {
            if (signature.startsWith("function ")) {
                signature = it.name() + signature.mid(9);
            } else {
                signature = signature.replace("QString", "string")
                                     .replace("QScriptValue", "value")
                                     .replace("GameObjectPtr", "GameObject")
                                     .replace("GameObject*", "GameObject")
                                     .replace("ScriptFunction", "function")
                                     .replace(",", ", ");
            }
            send(QString("  %1\n").arg(Util::highlight(signature)));
        }
    }
}
