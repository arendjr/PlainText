#include "buycommand.h"

#include "util.h"


#define super Command

BuyCommand::BuyCommand(QObject *parent) :
    super(parent) {

    setDescription("Buy an item from a character. Use *buy from <character>* to enquiry what goods "
                   "a character has for sale. Use *buy <item> from <character>* to buy a specific "
                   "item from a character.\n"
                   "\n"
                   "Examples: buy from shopkeeper, buy book from shopkeeper");
}

BuyCommand::~BuyCommand() {
}

void BuyCommand::execute(Player *player, const QString &command) {

    super::prepareExecute(player, command);

    GameObjectPtrList sellers;
    for (const GameObjectPtr &character : currentRoom()->characters()) {
        if (character->hasTrigger("onbuy")) {
            sellers << character;
        }
    }

    if (!requireSome(sellers, "There is noone that sells anything here.")) {
        return;
    }

    ObjectDescription itemDescription;
    if (peekWord() != "from") {
        itemDescription = takeObjectsDescription();
    }

    GameObjectPtr sellerPtr;
    if (peekWord() == "from") {
        takeWord();

        sellerPtr = takeObject(currentRoom()->characters());
    } else if (sellers.length() == 1) {
        sellerPtr = sellers[0];
    }

    if (!requireSome(sellerPtr, "Buy from who?")) {
        return;
    }

    Character *seller = sellerPtr.cast<Character *>();
    QString sellerName = seller->definiteName(currentRoom()->characters(), Capitalized);

    if (seller->sellableItems().isEmpty()) {
        send("%1 has nothing for sale.", sellerName);
        return;
    }

    if (itemDescription.name.isEmpty()) {
        seller->invokeTrigger("onbuy", player);
        return;
    }

    GameObjectPtrList items = objectsByDescription(itemDescription, seller->sellableItems());
    if (!requireSome(items, QString("%1 doesn't sell that.").arg(sellerName))) {
        return;
    }

    GameObjectPtrList boughtItems;
    for (const GameObjectPtr &itemPtr : items) {
        Item *item = itemPtr.cast<Item *>();
        QString itemName = item->definiteName(seller->sellableItems());

        if (item->cost() > player->gold()) {
            send("You don't have enough gold to buy %1.", itemName);
            continue;
        }

        if (player->inventoryWeight() + item->weight() > player->maxInventoryWeight()) {
            send("%1 is too heavy to carry.", Util::capitalize(itemName));
            continue;
        }

        if (!seller->invokeTrigger("onbuy", player, itemPtr)) {
            continue;
        }

        player->adjustGold(-item->cost());
        player->addInventoryItem(item->copy());

        boughtItems.append(itemPtr);
    }

    if (!boughtItems.isEmpty()) {
        send("You bought %1.", boughtItems.joinFancy());
    }
}
