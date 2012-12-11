
function Portal() {
}

Portal.prototype.lookAtBy = function(character) {

    var room = character.currentRoom;
    var name = this.nameFromRoom(room);

    var text;
    if (this.description.isEmpty()) {
        if (Util.isDirection(name)) {
            text = "You look %1.".arg(name);
        } else {
            text = "There's nothing special about the %1.".arg(name);
        }
    } else {
        text = this.descriptionFromRoom(room) + "\n";
    }

    if (this.canOpenFromRoom(room)) {
        text += "The %1 is %2.\n".arg(name, this.isOpen() ? "open" : "closed");
    }

    if (room === this.room) {
        character.direction = this.room2.position.minus(this.room.position);
    } else {
        character.direction = this.room.position.minus(this.room2.position);
    }

    return text;
};
