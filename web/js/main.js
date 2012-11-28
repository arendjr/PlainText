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


Object.defineProperty(Object.prototype, "contains", {
    "value": Object.prototype.hasOwnProperty,
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

String.prototype.contains = function(string) {

    return this.indexOf(string) > -1;
};

String.prototype.isEmpty = function() {

    return this.length === 0;
};

String.prototype.startsWith = function(string) {

    return this.substr(0, string.length) === string;
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
