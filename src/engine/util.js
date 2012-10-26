
var Color = {
    White: 0,
    Silver: 1,
    Gray: 2,
    Black: 3,
    Red: 4,
    Maroon: 5,
    Yellow: 6,
    Olive: 7,
    Lime: 8,
    Green: 9,
    Aqua: 10,
    Teal: 11,
    Blue: 12,
    Navy: 13,
    Fuchsia: 14,
    Purple: 15
};

var Options = {
    None: 0x0000,
    Capitalized: 0x0001,
    DefiniteArticles: 0x0002,
    IfNotLast: 0x0040
};


Array.prototype.append = function(value) {

    return this.push(value);
};

Array.prototype.clear = function() {

    return this.splice(0, this.length);
};

Array.prototype.contains = function(element) {

    return this.indexOf(element) > -1;
};

Array.prototype.containsAny = function(array) {

    return this.findFirstAmong(array) !== null;
};

Array.prototype.findFirstAmong = function(array) {

    for (var i = 0; i < this.length; i++) {
        for (var j = 0; j < array.length; j++) {
            if (this[i] === array[j]) {
                return this[i];
            }
        }
    }
    return null;
};

Array.prototype.first = function() {

    return this[0];
};

Array.prototype.insert = function(element) {

    if (!this.contains(element)) {
        this.push(element);
    }
};

Array.prototype.isEmpty = function() {

    return this.length === 0;
};

Array.prototype.joinFancy = function(options) {

    return Util.joinPtrList(this, options);
};

Array.prototype.last = function() {

    return this[this.length - 1];
};

Array.prototype.mid = function(begin, end) {

    return this.slice(begin, end - begin);
};

Array.prototype.named = function(name) {

    for (var i = 0; i < this.length; i++) {
        var element = this[i];
        if (element && typeof element === "object" && element.name === name) {
            return element;
        }
    }
    return undefined;
};

Array.prototype.randomElement = function() {

    return this[randomInt(0, this.length)];
};

Array.prototype.removeAll = function(value) {

    var numRemovals = 0;
    while (this.removeOne(value)) {
        numRemovals++;
    }
    return numRemovals;
};

Array.prototype.removeAt = function(index) {

    this.splice(index, 1);
};

Array.prototype.removeFirst = function() {

    this.splice(0, 1);
};

Array.prototype.removeLast = function() {

    this.splice(this.length - 1, 1);
};

Array.prototype.removeOne = function(value) {

    var index = this.indexOf(value);
    if (index !== -1) {
        this.splice(index, 1);
        return true;
    }
    return false;
};

Array.prototype.send = function(message) {

    for (var i = 0; i < this.length; i++) {
        var element = this[i];
        if (element && typeof element === "object" && element.send) {
            element.send(message);
        }
    }
};

Array.prototype.takeFirst = function() {

    return this.splice(0, 1)[0];
};

Array.prototype.takeLast = function() {

    return this.splice(this.length - 1, 1)[0];
};

Array.prototype.withId = function(id) {

    for (var i = 0; i < this.length; i++) {
        var element = this[i];
        if (element && typeof element === "object" && element.id === id) {
            return element;
        }
    }
    return undefined;
};

Array.prototype.withoutId = function(id) {

    var result = [];
    for (var i = 0; i < this.length; i++) {
        var element = this[i];
        if (element && typeof element === "object" && element.id !== id) {
            result.push(element);
        }
    }
    return result;
};


String.prototype.arg = function() {

    var result = this;

    var argNum;
    for (var i = 0; i < 10 && (argNum === undefined || argNum < arguments.length); i++) {
        var substr = "%" + i;
        var index = result.indexOf(substr);
        if (index > -1) {
            if (argNum === undefined) {
                argNum = 0;
            }

            while (index > -1) {
                result = result.replace(substr, arguments[argNum]);
                index = result.indexOf(substr, index);
            }
        }

        if (argNum !== undefined) {
            argNum++;
        }
    }

    return result;
}

String.prototype.capitalized = function() {

    return Util.capitalize(this);
};

String.prototype.contains = function(string) {

    return this.indexOf(string) > -1;
};

String.prototype.endsWith = function(string) {

    return this.substr(this.length - string.length) === string;
};

String.prototype.highlighted = function() {

    return Util.highlight(this);
};

String.prototype.isEmpty = function() {

    return this.length === 0;
};

String.prototype.left = function(n) {

    return this.substr(0, n);
};

String.prototype.mid = function(begin, end) {

    return this.substr(begin, end);
};

String.prototype.replaceAll = function(before, after) {

    var result = this;
    while (result.contains(before)) {
        result = result.replace(before, after);
    }
    return result;
};

String.prototype.right = function(n) {

    return this.substr(n);
};

String.prototype.startsWith = function(string) {

    return this.substr(0, string.length) === string;
};

String.prototype.toInt = function() {

    return parseInt(this, 10);
};

String.prototype.toLower = function() {

    return this.toLowerCase();
};

String.prototype.toUpper = function() {

    return this.toUpperCase();
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
