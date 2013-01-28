
function Room() {
}

Room.prototype.addGold = function(amount) {

    var goldItem;
    for (var i = 0, length = this.items.length; i < length; i++) {
        var item = this.items[i];
        if (item.name.endsWith("worth of gold")) {
            goldItem = item;
            break;
        }
    }

    if (goldItem) {
        goldItem.cost += amount;
    } else {
        goldItem = Realm.createObject("Item");
        goldItem.cost = amount;
        goldItem.portable = true;
        this.addItem(goldItem);
    }

    goldItem.name = "$%1 worth of gold".arg(amount);
};

Room.prototype.lookAtBy = function(character) {

    var text = "", i, length;

    if (!this.name.isEmpty()) {
        text += "\n" + this.name.colorized(Color.Teal) + "\n\n";
    }

    var itemGroups = {
        "left": [],
        "right": [],
        "ahead": [],
        "behind": [],
        "center": [],
        "above": [],
        "left wall": [],
        "right wall": []
    };
    this.items.forEach(function(item) {
        if (item.hidden) {
            return;
        }

        if (item.position[0] === 0 && item.position[1] === 0) {
            itemGroups["center"].append(item);
        } else {
            var angle = Math.atan2(item.position[1], item.position[0]) -
                        Math.atan2(character.direction[1], character.direction[0]);
            if (angle < -Math.PI) {
                angle += 2 * Math.PI;
            } else if (angle > Math.PI) {
                angle -= 2 * Math.PI;
            }

            if (angle > Math.PI * 3 / 4 || angle < -Math.PI * 3 / 4) {
                itemGroups["behind"].append(item);
            } else if (angle < Math.PI / 4 && angle > -Math.PI / 4) {
                itemGroups["ahead"].append(item);
            } else if (angle > 0) {
                itemGroups["right"].append(item);
            } else if (angle < 0) {
                itemGroups["left"].append(item);
            }
        }
    });

    var items = [];
    for (var key in itemGroups) {
        if (itemGroups.hasOwnProperty(key) && !itemGroups[key].isEmpty()) {
            var prefix;
            if (key === "left") {
                prefix = "To your left";
            } else if (key === "right") {
                prefix = "To your right";
            } else if (key === "ahead") {
                prefix = "Ahead of you, there";
            } else if (key === "behind") {
                prefix = "Behind you";
            } else {
                prefix = "There";
            }

            var itemText;
            var group = itemGroups[key];
            if (group.length === 1) {
                var item = group[0];
                itemText = "%1 is %2 %3.".arg(prefix, item.indefiniteArticle, item.name);
            } else {
                var plural = false;
                var firstName = group[0].name;
                for (i = 1, length = group.length; i < length; i++) {
                    if (group[i].name === firstName) {
                        plural = true;
                        break;
                    }
                }
                if (plural) {
                    itemText = "%1 are %2.".arg(prefix, group.joinFancy());
                } else {
                    itemText = "%1 is %2.".arg(prefix, group.joinFancy());
                }
            }

            itemText = itemText.replace("there is", "there's");
            items.prepend(itemText);
        }
    }

    var description;
    if (this.description.contains("[items]")) {
        description = this.description.replace("[items]", items.join(" "));
    } else {
        description = [this.description].concat(items).join(" ");
    }

    text += description + "\n";

    if (!this.portals.isEmpty()) {
        var exitNames = [];
        for (i = 0, length = this.portals.length; i < length; i++) {
            var portal = this.portals[i];

            if (portal.isHiddenFromRoom(this)) {
                continue;
            }

            exitNames.append(portal.nameFromRoom(this));
        }
        exitNames = Util.sortExitNames(exitNames);
        text += ("Obvious exits: " + exitNames.join(", ") + ".").colorized(Color.Green) + "\n";
    }

    var others = this.characters;
    others.removeOne(character);
    if (!others.isEmpty()) {
        text += "You see %1.\n".arg(others.joinFancy());
    }

    return text;
};

Room.prototype.portalNamed = function(name) {

    for (var i = 0, length = this.portals.length; i < length; i++) {
        var portal = this.portals[i];
        if (portal.nameFromRoom(this) === name) {
            return portal;
        }
    }
    return undefined;
};
