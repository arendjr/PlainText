
function Item() {
}

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
