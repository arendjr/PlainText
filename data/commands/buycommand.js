function BuyCommand() {
    Command.call(this);

    this.setDescription("Buy an item from a character. Use *buy from <character>* or *enquiry " +
                        "<character>* to enquiry what goods a character has for sale. Use *buy " +
                        "<item> from <character>* to buy a specific item from a character.\n" +
                        "\n" +
                        "Examples: buy from shopkeeper, enquiry shopkeeper, buy book from " +
                        "shopkeeper");
}

BuyCommand.prototype = new Command();
BuyCommand.prototype.constructor = BuyCommand;

BuyCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var sellers = [];
    this.currentRoom.characters.forEach(function(character) {
        if (character.hasTrigger("onbuy")) {
            sellers.append(character);
        }
    });

    if (!this.requireSome(sellers, "There is no one that sells anything here.")) {
        return;
    }

    var itemDescription;
    if (this.alias !== "enquiry" && this.peekWord() !== "from") {
        itemDescription = this.takeObjectsDescription();
    }

    var seller;
    if (this.alias === "enquiry") {
        seller = this.takeObject(this.currentRoom.characters);
    } else if (this.peekWord() === "from") {
        this.takeWord();

        seller = this.takeObject(this.currentRoom.characters);
    } else if (sellers.length === 1) {
        seller = sellers[0];
    }

    if (!seller) {
        if (this.alias === "enquiry") {
            player.send("Enquiry who?");
        } else {
            player.send("Buy from who?");
        }
        return;
    }

    var sellerName = seller.definiteName(this.currentRoom.characters, Options.Capitalized);

    if (seller.sellableItems.isEmpty()) {
        this.send("%1 has nothing for sale.", sellerName);
        return;
    }

    if (!itemDescription) {
        seller.invokeTrigger("onbuy", player);
        return;
    }

    var items = this.objectsByDescription(itemDescription, seller.sellableItems);
    if (!this.requireSome(items, "%1 doesn't sell that.".arg(sellerName))) {
        return;
    }

    var boughtItems = [];
    for (var i = 0, length = items.length; i < length; i++) {
        var item = items[i];
        var itemName = item.definiteName(seller.sellableItems);

        if (item.cost > player.gold) {
            this.send("You don't have enough gold to buy %1.", itemName);
            continue;
        }

        if (player.inventoryWeight + item.weight > player.maxInventoryWeight()) {
            this.send("%1 is too heavy to carry.", itemName.capitalized());
            continue;
        }

        if (!seller.invokeTrigger("onbuy", player, item)) {
            continue;
        }

        player.gold -= item.cost;
        player.addInventoryItem(item.copy());

        boughtItems.append(item);
    }

    if (!boughtItems.isEmpty()) {
        this.send("You bought %1.", boughtItems.joinFancy());
    }
}

CommandRegistry.registerCommand("buy", new BuyCommand());
CommandRegistry.registerCommand("enquiry", new BuyCommand());
