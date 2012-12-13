
function Player() {
}

Player.prototype.enteredRoom = function() {

    this.send(this.currentRoom.lookAtBy(this));
}
