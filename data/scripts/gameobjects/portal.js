
function Portal() {
}

Portal.prototype.nameWithDestinationFromRoom = function(room) {

    var name = this.nameFromRoom(room);
    var destination = this.destinationFromRoom(room);
    if (destination.isEmpty()) {
        if (name === "door") {
            return "a door";
        } else {
            return "the " + name;
        }
    } else {
        return "the %1 to %2".arg(name, destination);
    }
};

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
                text = "You look at the %1.".arg(name);
            }
        } else {
            text = description;
        }
    } else {
        if (description.isEmpty()) {
            text = "The %1 leads to %2.".arg(name, destination);
        } else {
            text = "The %1 leads to %2. %3".arg(name, destination, description);
        }
    }

    if (this.canOpenFromRoom(room)) {
        if (text === "") {
            text = "The %1 is %2.".arg(name, this.open ? "open" : "closed");
        } else {
            text += " It's %1.".arg(this.open ? "open" : "closed");
        }
    }

    if (room === this.room) {
        character.direction = this.room2.position.minus(this.room.position);
    } else {
        character.direction = this.room.position.minus(this.room2.position);
    }

    return text;
};
