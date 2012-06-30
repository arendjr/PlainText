
Array.prototype.named = function(name) {

    for (var i = 0; i < this.length; i++) {
        var element = this[i];
        if (element && typeof element === "object" && element.name === name) {
            return element;
        }
    }
    return undefined;
}

Array.prototype.withId = function(id) {

    for (var i = 0; i < this.length; i++) {
        var element = this[i];
        if (element && typeof element === "object" && element.id === id) {
            return element;
        }
    }
    return undefined;
}

Array.prototype.withoutId = function(id) {

    var result = [];
    for (var i = 0; i < this.length; i++) {
        var element = this[i];
        if (element && typeof element === "object" && element.id !== id) {
            result.push(element);
        }
    }
    return result;
}

Array.prototype.randomElement = function() {

    return this[randomInt(0, this.length)];
}

String.prototype.capitalized = function() {

    return Util.capitalize(this);
}

String.prototype.highlighted = function() {

    return Util.highlight(this);
}

String.prototype.trimmed = function() {

    return this.replace(/^\s+|\s+$/g, "");
};
