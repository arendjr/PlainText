
function Player() {
}

Player.prototype.enteredRoom = function() {

    this.send(this.currentRoom.lookAtBy(this));
};

Player.prototype.sendSellableItemsList = function(items) {

    var message = "";
    items.forEach(function(item) {
        message += "  %1$%2\n".arg(item.name.leftJustified(30)).arg(item.cost);
    });
    this.send(message);
};
