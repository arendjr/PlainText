
/**
 * Constructor.
 *
 * @note Don't instantiate this class directly, use Realm.createObject("Portal") instead.
 */
function Portal() {
}

/**
 * Returns a minimal description of the portal composed of the portal's name and its destination.
 *
 * @param room Room from which the portal is being observed.
 *
 * @return string
 */

Portal.prototype.nameWithDestinationFromRoom = function(room) {

    var name = this.nameFromRoom(room);
    var destination = this.destinationFromRoom(room);
    if (destination.isEmpty()) {
        if (name === "door" || name === "tent") {
            return "a " + name;
        } else {
            return "the " + name;
        }
    } else {
        return "the %1 to %2".arg(name, destination);
    }
};

/**
 * Returns the description of the portal as seen by a specific character.
 *
 * @param character The character looking at the portal.
 *
 * @return string
 */
Portal.prototype.lookAtBy = function(character) {

    var room = character.currentRoom;
    var name = this.nameFromRoom(room);
    var description = this.descriptionFromRoom(room);
    var destination = this.destinationFromRoom(room);

    var text;
    if (Util.isDirection(name)) {
        text = "You look %1.".arg(name);

        if (!destination.isEmpty()) {
            text += " It leads to %1.".arg(destination);
        }
    } else {
        if (destination.isEmpty()) {
            text = "You look at the %1.".arg(name);
        } else {
            text = "You look at the %1 to %2.".arg(name, destination);
        }
    }

    if (!description.isEmpty()) {
        text += " " + description;
    }

    if (this.canOpenFromRoom(room)) {
        text += " It's %1.".arg(this.open ? "open" : "closed");
    }

    if (room === this.room) {
        character.direction = this.room2.position.minus(this.room.position);
    } else {
        character.direction = this.room.position.minus(this.room2.position);
    }

    return text;
};
