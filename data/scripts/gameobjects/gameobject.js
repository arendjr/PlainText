
function GameObject() {
}

GameObject.prototype.toString = function() {

    return this.objectType + "(name = \"" + this.name + "\")";
};
