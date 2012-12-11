
function Container() {
}

Container.prototype.lookAtBy = function(character) {

    if (this.description.isEmpty()) {
        return "The %1 contains %2.".arg(this.name, this.items.joinFancy());
    } else {
        return this.description + "\n" +
               "It contains %1.".arg(this.items.joinFancy());
    }
};
