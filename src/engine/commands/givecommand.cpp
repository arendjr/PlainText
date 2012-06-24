#include "givecommand.h"

#include "engine/util.h"


GiveCommand::GiveCommand(Player *player, QObject *parent) :
    Command(player, parent) {

    setDescription("Give an item or gold from your inventory to another "
                   "character.\n"
                   "\n"
                   "Example: give stick earl");
}

GiveCommand::~GiveCommand() {
}

void GiveCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Give what?")) {
        return;
    }

    takeWord("the");

    GameObjectPtrList items = takeObjects(player()->inventory());
    if (!requireSome(items, "You don't have that.")) {
        return;
    }

    takeWord("to", IfNotLast);

    GameObjectPtrList recipients = takeObjects(currentArea()->characters());
    if (!requireSome(recipients, "That recipient is not here.")) {
        return;
    }

    Character *recipient = recipients[0].cast<Character *>();
    gopl_foreach (item, items) {
        recipient->addInventoryItem(item);
        player()->removeInventoryItem(item);
    }

    QString recipientName = recipient->definiteName(recipients);
    QString itemsDescription = Util::joinItems(items);
    player()->send(QString("You give %1 %2.").arg(recipientName, itemsDescription));
    recipient->send(QString("%1 gives you %2.").arg(player()->name(), itemsDescription));

    Util::sendOthers(currentArea()->players(),
                     QString("%1 gives %2 %3.").arg(player()->name(), recipientName, itemsDescription),
                     player(), recipient);
}
