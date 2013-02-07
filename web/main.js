/*global require:false*/

Array.prototype.append = function(value) {

    if (value instanceof Array) {
        for (var i = 0, l = value.length; i < l; i++) {
            this.push(value[i]);
        }
    } else {
        return this.push(value);
    }
};

Array.prototype.clear = function() {

    return this.splice(0, this.length);
};

Array.prototype.clone = function() {

    return this.slice(0);
};

Array.prototype.contains = function(element) {

    return this.indexOf(element) > -1;
};

Array.prototype.insert = function(element) {

    if (!this.contains(element)) {
        this.push(element);
    }
};

Array.prototype.isEmpty = function() {

    return this.length === 0;
};

Array.prototype.removeAll = function(value) {

    var numRemovals = 0;
    while (this.removeOne(value)) {
        numRemovals++;
    }
    return numRemovals;
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


Object.defineProperty(Object.prototype, "clone", {
    "value": function() {
        var result = {};
        for (var key in this) {
            if (this.hasOwnProperty(key)) {
                result[key] = this[key];
            }
        }
        return result;
    },
    "configurable": true,
    "writable": true
});

Object.defineProperty(Object.prototype, "contains", {
    "value": Object.prototype.hasOwnProperty,
    "configurable": true,
    "writable": true
});

Object.defineProperty(Object.prototype, "isEmpty", {
    "value": function() { for (var key in this) { return false; } return true; },
    "configurable": true,
    "writable": true
});

Object.defineProperty(Object.prototype, "keys", {
    "value": function() { return Object.keys(this); },
    "configurable": true,
    "writable": true
});


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
};

String.prototype.capitalized = function() {

    return this.substr(0, 1).toUpperCase() + this.substr(1);
};

String.prototype.contains = function(string) {

    return this.indexOf(string) > -1;
};

String.prototype.endsWith = function(string) {

    return this.substr(this.length - string.length) === string;
};

String.prototype.isEmpty = function() {

    return this.length === 0;
};

String.prototype.left = function(n) {

    return this.substr(0, n);
};

String.prototype.leftJustified = function(n, fill) {

    fill = fill || " ";
    var padding = "";
    for (var i = n - this.length; i > 0; i--) {
        padding += fill;
    }
    return this + padding;
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

String.prototype.rightJustified = function(n, fill) {

    fill = fill || " ";
    var padding = "";
    for (var i = n - this.length; i > 0; i--) {
        padding += fill;
    }
    return padding + this;
};

String.prototype.startsWith = function(string) {

    return this.substr(0, string.length) === string;
};

String.prototype.toDouble = function() {

    var value = parseFloat(this);
    if (isNaN(value)) {
        value = 0.0;
    }
    return value;
};

String.prototype.toInt = function() {

    var value = parseInt(this, 10);
    if (isNaN(value)) {
        value = 0;
    }
    return value;
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


function init() {

    require(["controller"]);

    if ((window.notifications && window.notifications.requestPermission) ||
        (window.webkitNotifications && window.webkitNotifications.requestPermission)) {
        require(["notifications"]);
    }
}

init();
