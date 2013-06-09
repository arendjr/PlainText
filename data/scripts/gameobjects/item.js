
/**
 * Constructor.
 *
 * @note Don't instantiate this class directly, use Realm.createObject("Item") instead.
 */
function Item() {
}

/**
 * Returns the description of the item as seen by a specific character.
 *
 * @param character The character looking at the item.
 *
 * @return string
 */
Item.prototype.lookAtBy = function(character) {

    var text = "You look at %1.".arg(this.definiteName([]));
    if (this.description) {
        text += " " + this.description;
    }

    if (!this.position.equals([0, 0, 0])) {
        character.direction = this.position;
    }

    return text;
};
