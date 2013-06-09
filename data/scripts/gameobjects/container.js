
/**
 * Constructor.
 *
 * @note Don't instantiate this class directly, use Realm.createObject("Container") instead.
 */
function Container() {
}

/**
 * Returns the description of the item as seen by a specific character.
 *
 * @param character The character looking at the item.
 *
 * @return string
 */
Container.prototype.lookAtBy = function(character) {

    if (this.description.isEmpty()) {
        return "The %1 contains %2.".arg(this.name, this.items.joinFancy());
    } else {
        return this.description + "\n" +
               "It contains %1.".arg(this.items.joinFancy());
    }
};
