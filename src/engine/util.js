
Array.prototype.contains = function(element) {

    return this.indexOf(element) > -1;
};

Array.prototype.isEmpty = function() {

    return this.length === 0;
};

Array.prototype.named = function(name) {

    for (var i = 0; i < this.length; i++) {
        var element = this[i];
        if (element && typeof element === "object" && element.name === name) {
            return element;
        }
    }
    return undefined;
}

Array.prototype.send = function(message) {

    for (var i = 0; i < this.length; i++) {
        var element = this[i];
        if (element && typeof element === "object" && element.send) {
            element.send(message);
        }
    }
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

Array.prototype.findFirstAmong = function(array) {

    for (var i = 0; i < this.length; i++) {
        for (var j = 0; j < array.length; j++) {
            if (this[i] === array[j]) {
                return this[i];
            }
        }
    }
    return null;
}

Array.prototype.randomElement = function() {

    return this[randomInt(0, this.length)];
}

String.prototype.capitalized = function() {

    return Util.capitalize(this);
}

String.prototype.contains = function(string) {

    return this.indexOf(string) > -1;
};

String.prototype.endsWith = function(string) {

    return this.substr(this.length - string.length) === string;
};

String.prototype.highlighted = function() {

    return Util.highlight(this);
}

String.prototype.isEmpty = function() {

    return this.length === 0;
};

String.prototype.startsWith = function(string) {

    return this.substr(0, string.length) === string;
};

String.prototype.trimmed = function() {

    return this.replace(/^\s+|\s+$/g, "");
};

function $(identifier) {

    var split = identifier.split(":");
    return Realm.getObject(split[0], parseInt(split[1], 10));
}

function byChance(n, p) {

    return randomInt(0, p) < n;
}
