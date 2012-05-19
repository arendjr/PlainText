#include "givecommand.h"

#include <QRegExp>

#include "engine/character.h"
#include "engine/item.h"
#include "engine/util.h"


GiveCommand::GiveCommand(Player *character, QObject *parent) :
    Command(character, parent) {
}

GiveCommand::~GiveCommand() {
}

void GiveCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Give what?")) {
        return;
    }

    GameObjectPtrList items = takeObjects(player()->inventory());
    if (!requireSome(items, "You don't have that.")) {
        return;
    }

    takeWord(QRegExp("to"));

    GameObjectPtrList recipients = takeObjects(currentArea()->characters());
    if (!requireUnique(recipients, "Recipient is not here.", "Recipient is not unique.")) {
        return;
    }

    Character *recipient = recipients[0].cast<Character *>();
    foreach (const GameObjectPtr &item, items) {
        recipient->addInventoryItem(item);
        player()->removeInventoryItem(item);
    }

    QString itemsDescription = Util::joinItems(items);
    player()->send(QString("You give %1 %2.").arg(recipient->name(), itemsDescription));
    recipient->send(QString("%1 gives you %2.").arg(player()->name(), itemsDescription));

    Util::sendOthers(currentArea()->players(),
                     QString("%1 gives %2 %3.").arg(player()->name(), recipient->name(), itemsDescription),
                     player(), recipient);
}
