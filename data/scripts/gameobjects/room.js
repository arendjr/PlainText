
/**
 * Constructor.
 *
 * @note Don't instantiate this class directly, use Realm.createObject("Room") instead.
 */
function Room() {
}

/**
 * Adds an amount of gold to the room.
 *
 * @param amount The amount of gold to add to the room.
 */
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

/**
 * Returns the description of the room as seen by a specific character.
 *
 * @param character The character looking at the room.
 *
 * @return string
 */
Room.prototype.lookAtBy = function(character) {

    var text = "", self = this;

    if (!this.name.isEmpty()) {
        text += "\n" + this.name.colorized(Color.Teal) + "\n\n";
    }

    var flags = this.flags.split("|");
    var hasDynamicPortals = !flags.contains("OmitDynamicPortalsFromDescription");
    var hasDistantCharacters = !flags.contains("OmitDistantCharactersFromDescription");

    var itemGroups = VisualUtil.divideItemsIntoGroups(this.items, character.direction);

    var portalGroups;
    if (hasDynamicPortals || hasDistantCharacters) {
        portalGroups = VisualUtil.dividePortalsAndCharactersIntoGroups(character, this);
    }

    var itemTexts = [];
    for (var key in itemGroups) {
        if (!itemGroups[key].isEmpty() || hasDynamicPortals && !portalGroups[key].isEmpty()) {
            var itemGroup = itemGroups[key];
            var plural = itemGroup.firstItemIsPlural();

            var combinedItems = Util.combinePtrList(itemGroup);

            if (hasDynamicPortals) {
                portalGroups[key].forEach(function(portal) {
                    combinedItems.append(portal.nameWithDestinationFromRoom(self));
                });
            }

            var groupDescription = VisualUtil.descriptionForGroup(key);
            var prefix = groupDescription[0];
            var helperVerb = groupDescription[plural ? 2 : 1];
            itemTexts.append("%1 %2 %3.".arg(prefix, helperVerb, Util.joinFancy(combinedItems))
                                        .replace("there is", "there's"));
        }
    }

    var characterText = "";
    if (hasDistantCharacters && portalGroups.hasOwnProperty("characters")) {
        var characters = portalGroups["characters"];
        characterText = VisualUtil.describeCharactersRelativeTo(characters, character);
    }

    text += this.description;
    if (!itemTexts.isEmpty()) {
        if (!text.endsWith(" ") && !text.endsWith("\n")) {
            text += " ";
        }
        text += itemTexts.join(" ");
    }
    if (!characterText.isEmpty()) {
        if (!text.endsWith(" ") && !text.endsWith("\n")) {
            text += " ";
        }
        text += characterText;
    }
    text += "\n";

    var exitNames = [];
    this.portals.forEach(function(portal) {
        if (!portal.isHiddenFromRoom(self)) {
            exitNames.append(portal.nameFromRoom(self));
        }
    });
    if (!exitNames.isEmpty()) {
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

/**
 * Returns a portal that is connected to the room by its name as perceived from this room.
 *
 * @return object
 */
Room.prototype.portalNamed = function(name) {

    for (var i = 0, length = this.portals.length; i < length; i++) {
        var portal = this.portals[i];
        if (portal.nameFromRoom(this) === name) {
            return portal;
        }
    }
    return null;
};
