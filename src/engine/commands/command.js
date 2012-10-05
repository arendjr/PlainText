
function Command() {

    this._description = "";
    this._player = null;
    this._currentArea = null;
    this._words = [];
    this._alias = "";
}

Command.prototype.description = function() {

    return this._description;
};

Command.prototype.setDescription = function(description) {

    this._description = Util.processHighlights(description);
};

Command.prototype.execute = null;

Command.prototype.prepareExecute = function(player, command) {

    this.setPlayer(player);
    this.setCommand(command);

    this._alias = this.takeWord();
};

Command.prototype.setPlayer = function(player) {

    this._player = player;
    this._currentArea = player.currentArea;
};

Command.prototype.setCommand = function(command) {

    var words = command.trimmed().split(/\s+/);

    for (var i = 0, length = words.length; i < length; i++) {
        var word = words[i];
        if (word.startsWith("\"")) {
            while (i < words.length - 1 && !word.endsWith("\"")) {
                word += " " + words[i + 1];
                words.removeAt(i);
            }
            if (word.endsWith("\"")) {
                words[i] = word.mid(1, word.length - 2);
            } else {
                words[i] = word.mid(1);
            }
        }
    }

    this._words = words;
};

Command.prototype.prependWord = function(word) {

    this._words.splice(0, 0, word);
};

Command.prototype.appendWord = function(word) {

    this._words.push(word);
};

Command.prototype.numWordsLeft = function() {

    return this._words.length;
};

Command.prototype.hasWordsLeft = function() {

    return this._words.length > 0;
};

Command.prototype.assertWordsLeft = function(noneLeftText) {

    if (!this.hasWordsLeft()) {
        this._player.send(noneLeftText);
        return false;
    } else {
        return true;
    }
}

Command.prototype.takeWord = function(pattern, options) {

    if (this._words.length > 0) {
        if (typeof pattern === "string") {
            pattern = new RegExp("^" + pattern + "$");
            if (options & Options.IfNotLast && this._words.length === 1) {
                return "";
            }
            if (pattern.test(this._words[0])) {
                return this._words.takeFirst();
            }
        } else {
            options = pattern;
            if (options & Options.IfNotLast && this._words.length === 1) {
                return "";
            }
            return this._words.takeFirst();
        }
    }

    return "";
};

Command.prototype.takeObject = function(pool) {

    var description = this.takeObjectsDescription();
    if (!description.first.isEmpty()) {
        var objects = this.objectsByDescription(description, pool);
        if (objects.length > 0) {
            return objects[0];
        }
    }

    return null;
};

Command.prototype.takeObjects = function(pool) {

    var description = this.takeObjectsDescription();
    if (!description.first.isEmpty()) {
        return this.objectsByDescription(description, pool);
    }

    return [];
}

Command.prototype.takeObjectsDescription = function() {

    var description = { first: "", second: 0 };
    if (this._words.length > 0) {
        description.first = this._words.takeFirst().toLower();
        if (this._words.length > 0) {
            if (description.first === "first") {
                description.first = this._words.takeFirst().toLower();
                description.second = 1;
            } else if (description.first === "second") {
                description.first = this._words.takeFirst().toLower();
                description.second = 2;
            } else if (description.first === "third") {
                description.first = this._words.takeFirst().toLower();
                description.second = 3;
            } else if (description.first === "fourth") {
                description.first = this._words.takeFirst().toLower();
                description.second = 4;
            } else {
                description.second = this._words.first().toInt();
                if (description.second > 0) {
                    this._words.removeFirst();
                }
            }
        }
    }
    return description;
};

Command.prototype.takeRest = function() {

    var rest = this._words.join(" ");
    this._words.clear();
    return rest;
};

Command.prototype.objectsByDescription = function(description, pool) {

    var objects = [];
    if (description.first === "all") {
        objects = pool;
    } else {
        for (var i = 0, length = pool.length; i < length; i++) {
            var object = pool[i];
            var words = object.name.toLower().split(" ");
            for (var j = 0; j < words.length; j++) {
                var word = words[j];
                if (word.startsWith(description.first)) {
                    objects.push(object);
                    break;
                }
            }
        }
    }
    if (description.second > 0) {
        if (description.second <= objects.length) {
            var selected = objects[description.second - 1];
            objects = [ selected ];
        } else {
            objects.clear();
        }
    }
    return objects;
};

Command.prototype.requireSome = function(objects, tooFewText) {

    if (objects === null || objects.length === 0) {
        this.send(tooFewText);
        return false;
    } else {
        return true;
    }
};

Command.prototype.requireUnique = function(objects, tooFewText, tooManyText) {

    if (objects.length === 0) {
        this.send(tooFewText);
        return false;
    } else if (objects.length > 1) {
        this.send(tooManyText);
        return false;
    } else {
        return true;
    }
};

Command.prototype.send = function(message) {

    this._player.send(message);
};
