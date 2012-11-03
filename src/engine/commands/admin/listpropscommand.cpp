#include "listpropscommand.h"

#include "conversionutil.h"
#include "util.h"


#define super AdminCommand

ListPropsCommand::ListPropsCommand(QObject *parent) :
    super(parent) {

    setDescription("List all properties of an object, with their values.\n"
                   "\n"
                   "Usage: list-props <object-name> [#]");
}

ListPropsCommand::~ListPropsCommand() {
}

void ListPropsCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtrList objects = takeObjects(currentRoom()->objects());
    if (!requireUnique(objects, "Object not found.", "Object is not unique.")) {
        return;
    }

    GameObjectPtr object = objects[0];
    send(QString("These are all known properties of %1:\n"
                 "\n").arg(Util::highlight(QString("object #%1").arg(object->id()))));

    const int lineLength = 45;

    for (const QMetaProperty &metaProperty : object->metaProperties()) {
        const char *name = metaProperty.name();
        if (strcmp(name, "id") == 0) {
            continue;
        }

        QString value = ConversionUtil::toUserString(object->property(name));

        if (!metaProperty.isWritable()) {
            value += " (read-only)";
        }

        QStringList valueLines = value.split('\n');
        for (int i = 0; i < valueLines.length(); i++) {
            if (valueLines[i].length() > lineLength) {
                QStringList lines = Util::splitLines(valueLines[i], lineLength);
                valueLines.removeAt(i);
                for (int j = 0; j < lines.length(); j++, i++) {
                    valueLines.insert(i, lines[j]);
                }
            }
        }

        send("  " + Util::highlight(QString(name).leftJustified(30)) + "  " + valueLines[0]);
        for (int i = 1; i < valueLines.size(); i++) {
            send("                                  " + valueLines[i]);
        }
    }

    send("\n");
}
