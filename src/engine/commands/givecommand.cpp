#include "givecommand.h"

#include <QRegExp>

#include "engine/character.h"
#include "engine/item.h"
#include "engine/util.h"


GiveCommand::GiveCommand(Character *character, QObject *parent) :
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

    GameObjectPtrList items = takeObjects(character()->inventory());
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
        character()->removeInventoryItem(item);
    }

    QString itemsDescription = Util::joinItems(items);
    character()->send(QString("You give %1 %2.").arg(recipient->name(), itemsDescription));
    recipient->send(QString("%1 gives you %2.").arg(character()->name(), itemsDescription));

    Util::sendOthers(currentArea()->characters(),
                     QString("%1 gives %2 %3.").arg(character()->name(), recipient->name(), itemsDescription),
                     character(), recipient);
}
