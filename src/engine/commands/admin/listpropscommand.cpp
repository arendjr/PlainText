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

void ListPropsCommand::execute(Character *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtr object = takeObject(currentRoom()->exits() + currentRoom()->characters() +
                                      currentRoom()->items());
    if (!requireSome(object, "Object not found.")) {
        return;
    }

    send("These are all known properties of %1:\n\n",
         Util::highlight(QString("object #%1").arg(object->id())));

    const int lineLength = 45;

    for (const QMetaProperty &metaProperty : object->metaProperties()) {
        const char *propertyName = metaProperty.name();
        QString name(propertyName);
        if (name == "id" || name.contains("password")) {
            continue;
        }

        QString value = ConversionUtil::toUserString(object->property(propertyName));

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

        send("  " + Util::highlight(name.leftJustified(30)) + "  " + valueLines[0]);
        for (int i = 1; i < valueLines.size(); i++) {
            send("                                  " + valueLines[i]);
        }
    }

    send("\n");
}
