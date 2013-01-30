
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

    function prefixForItemKey(key) {
        switch (key) {
            case "left":        return [ "To your left", "is", "are" ];
            case "right":       return [ "To your right", "is", "are" ];
            case "ahead":       return [ "Ahead of you, there", "is", "are" ];
            case "behind":      return [ "Behind you", "is", "are" ];
            case "above":       return [ "Above you", "is", "are" ];
            case "left wall":   return [ "On the left wall", "hangs", "hang" ];
            case "right wall":  return [ "On the right wall", "hangs", "hang" ];
            case "wall":        return [ "On the wall", "hangs", "hang" ];
            case "ceiling":     return [ "From the ceiling", "hangs", "hang" ];
            default:            return [ "There", "is", "are" ];
        }
    }

    var itemGroups = {
        "left": [],
        "right": [],
        "ahead": [],
        "behind": [],
        "center": [],
        "above": [],
        "left wall": [],
        "right wall": [],
        "wall": [],
        "ceiling": []
    };
    var portalGroups = itemGroups.clone();

    var self = this;
    this.items.forEach(function(item) {
        if (item.hidden) {
            return;
        }

        var flags = item.flags.split("|");
        var angle = Util.angleBetweenDirectionAndPosition(character.direction, item.position);

        if (flags.contains("AttachedToCeiling")) {
            itemGroups["ceiling"].append(item);
        } else if (flags.contains("AttachedToWall")) {
            if ((item.position[0] === 0 && item.position[1] === 0) ||
                (angle > Math.PI * 3 / 4 || angle < -Math.PI * 3 / 4) ||
                (angle < Math.PI / 4 && angle > -Math.PI / 4)) {
                itemGroups["wall"].append(item);
            } else if (angle > 0) {
                itemGroups["right wall"].append(item);
            } else {
                itemGroups["left wall"].append(item);
            }
        } else if (item.position[0] === 0 && item.position[1] === 0) {
            itemGroups["center"].append(item);
        } else {
            if (angle > Math.PI * 3 / 4 || angle < -Math.PI * 3 / 4) {
                itemGroups["behind"].append(item);
            } else if (angle < Math.PI / 4 && angle > -Math.PI / 4) {
                itemGroups["ahead"].append(item);
            } else if (angle > 0) {
                itemGroups["right"].append(item);
            } else {
                itemGroups["left"].append(item);
            }
        }
    });
    this.portals.forEach(function(portal) {
        var name = portal.nameFromRoom(self);
        if (Util.isDirection(name) || name === "out" || portal.isHiddenFromRoom(self)) {
            return;
        }

        var angle = Util.angleBetweenDirectionAndPosition(character.direction,
                                                          portal.position.minus(self.position));

        if (angle > Math.PI * 3 / 4 || angle < -Math.PI * 3 / 4) {
            portalGroups["behind"].append(portal);
        } else if (angle < Math.PI / 4 && angle > -Math.PI / 4) {
            portalGroups["ahead"].append(portal);
        } else if (angle > 0) {
            portalGroups["right"].append(portal);
        } else {
            portalGroups["left"].append(portal);
        }
    });

    var items = [];
    for (var key in itemGroups) {
        if (itemGroups.hasOwnProperty(key) &&
            (!itemGroups[key].isEmpty() || !portalGroups[key].isEmpty())) {
            var prefix = prefixForItemKey(key);

            var plural = false;
            var itemGroup = itemGroups[key];
            if (itemGroup.length > 0) {
                if (itemGroup[0].flags.split("|").contains("ImpliedPlural")) {
                    plural = true;
                } else {
                    var firstName = itemGroup[0].name;
                    for (i = 1, length = itemGroup.length; i < length; i++) {
                        if (itemGroup[i].name === firstName) {
                            plural = true;
                            break;
                        }
                    }
                }
            }

            var combinedItems = Util.combinePtrList(itemGroup);

            var portalGroup = portalGroups[key];
            for (i = 0, length = portalGroup.length; i < length; i++) {
                var portal = portalGroup[i];
                var name = portal.nameFromRoom(self);
                var destination = portal.destinationFromRoom(self);
                if (destination.isEmpty()) {
                    if (name === "door") {
                        combinedItems.append("a door");
                    } else {
                        combinedItems.append("the " + name);
                    }
                } else {
                    combinedItems.append("the %1 to %2".arg(name, destination));
                }
            }

            var itemText;
            if (plural) {
                itemText = "%1 %2 %3.".arg(prefix[0], prefix[2], Util.joinFancy(combinedItems));
            } else {
                itemText = "%1 %2 %3.".arg(prefix[0], prefix[1], Util.joinFancy(combinedItems));
            }

            itemText = itemText.replace("there is", "there's");
            items.append(itemText);
        }
    }

    var description;
    if (this.description.contains("{{items}}") || this.description.contains("{{none}}")) {
        description = this.description.replace("{{items}}", items.join(" "))
                                      .replace("{{none}}", "");
    } else {
        description = [this.description].concat(items).join(" ");
    }

    text += description + "\n";

    if (!this.portals.isEmpty()) {
        var exitNames = [];
        for (i = 0, length = this.portals.length; i < length; i++) {
            portal = this.portals[i];

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
