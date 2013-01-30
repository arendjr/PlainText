
function Portal() {
}

Portal.prototype.lookAtBy = function(character) {

    var room = character.currentRoom;
    var name = this.nameFromRoom(room);
    var description = this.descriptionFromRoom(room);
    var destination = this.destinationFromRoom(room);

    var text = "";
    if (destination.isEmpty()) {
        if (description.isEmpty()) {
            if (Util.isDirection(name)) {
                text = "You look %1.".arg(name);
            } else if (!this.canOpenFromRoom(room)) {
                text = "There's nothing special about the %1.".arg(name);
            }
        } else {
            text = description + "\n";
        }
    } else {
        if (description.isEmpty()) {
            text = "The %1 leads to %2.\n".arg(name, destination);
        } else {
            text = "The %1 leads to %2. %3\n".arg(name, destination, description);
        }
    }

    if (this.canOpenFromRoom(room)) {
        text += "The %1 is %2.\n".arg(name, this.open ? "open" : "closed");
    }

    if (room === this.room) {
        character.direction = this.room2.position.minus(this.room.position);
    } else {
        character.direction = this.room.position.minus(this.room2.position);
    }

    return text;
};
