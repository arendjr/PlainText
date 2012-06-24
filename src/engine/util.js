
Array.prototype.named = function(name) {

    var result = [];
    for (var i = 0; i < this.length; i++) {
        var element = this[i];
        if (element && typeof element === "object" && element.name === name) {
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
