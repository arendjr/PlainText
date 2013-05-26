#include "triggersetcommand.h"

#include "realm.h"
#include "util.h"


#define super ApiCommand

TriggerSetCommand::TriggerSetCommand(QObject *parent) :
    super(parent) {

    setDescription("Syntax: api-trigger-set <request-id> <object-id> <trigger-name> "
                   "<trigger-source>");
}

TriggerSetCommand::~TriggerSetCommand() {
}

void TriggerSetCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = realm()->getObject(GameObjectType::Unknown, takeWord().toInt());
    if (object.isNull()) {
        sendError(404, "Object not found");
        return;
    }

    QString triggerName = takeWord().toLower();
    if (!triggerName.startsWith("on")) {
        sendError(400, "Invalid trigger name");
        return;
    }

    QString source = takeRest();
    if (source.isEmpty()) {
        sendError(400, "Missing trigger source");
        return;
    }

    if (source.startsWith("function")) {
        source = "(" + source + ")";
    }

    ScriptFunction trigger;
    ScriptFunction::fromUserString(source, trigger);
    object->setTrigger(triggerName, trigger);

    sendReply(QString("Trigger %1 set.").arg(triggerName));

    if (triggerName == "oninit" || triggerName == "onspawn") {
        object->killAllTimers();
        object->invokeTrigger("oninit");
        object->invokeTrigger("onspawn");
        send(QString("%1 reinitialized and respawn emulated.")
             .arg(Util::highlight(QString("Object #%1").arg(object->id()))));
    }
}

void TriggerSetCommand::setCommand(const QString &command) {

    static QRegExp whitespace("\\s+");

    QStringList words;
    words.append(command.section(whitespace, 0, 0));
    words.append(command.section(whitespace, 1, 1));
    words.append(command.section(whitespace, 2, 2));
    words.append(command.section(whitespace, 3, 3));
    if (words.last().toInt() > 0) {
        words.append(command.section(whitespace, 4, 4));
        words.append(command.section(whitespace, 5));
    } else {
        words.append(command.section(whitespace, 4));
    }
    setWords(words);
}
