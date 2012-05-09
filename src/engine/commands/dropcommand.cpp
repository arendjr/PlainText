#include "dropcommand.h"

#include "engine/character.h"
#include "engine/item.h"
#include "engine/util.h"


DropCommand::DropCommand(Character *character, QObject *parent) :
    Command(character, parent) {
}

DropCommand::~DropCommand() {
}

void DropCommand::execute(const QString &command) {

    setCommand(command);

    /*QString alias = */takeWord();
    if (!assertWordsLeft("Drop what?")) {
        return;
    }

    GameObjectPtrList items = takeObjects(character()->inventory());
    if (!requireSome(items, "You don't have that.")) {
        return;
    }

    foreach (const GameObjectPtr &item, items) {
        currentArea()->addItem(item);
        character()->removeInventoryItem(item);
    }

    QString itemsDescription = Util::joinItems(items, DefiniteArticle);
    character()->send(QString("You drop %2.").arg(itemsDescription));

    Util::sendOthers(currentArea()->characters(),
                     QString("%1 drops %2.").arg(character()->name(), itemsDescription),
                     character());
}
