
/**
 * Constructor.
 *
 * @note Don't instantiate this class directly, use Realm.createObject("Player") instead.
 */
function Player() {
}

/**
 * This method is called whenever the player has entered a room.
 */
Player.prototype.enteredRoom = function() {

    this.send(this.currentRoom.lookAtBy(this));
};

/**
 * Sends the player a list of items formatted with their cost next to it.
 */
Player.prototype.sendSellableItemsList = function(items) {

    var message = "";
    items.forEach(function(item) {
        message += "  %1$%2\n".arg(item.name.leftJustified(30)).arg(item.cost);
    });
    this.send(message);
};
