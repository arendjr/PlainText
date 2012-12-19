
function Command() {

    this.description = "";
    this.player = null;
    this.currentRoom = null;
    this.words = [];
    this.alias = "";
}

Command.prototype.setDescription = function(description) {

    this.description = Util.processHighlights(description);
};

Command.prototype.execute = null;

Command.prototype.prepareExecute = function(player, command) {

    this.setPlayer(player);
    this.setCommand(command);

    this.alias = this.takeWord();
};

Command.prototype.setPlayer = function(player) {

    this.player = player;
    this.currentRoom = player.currentRoom;
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

    this.words = words;
};

Command.prototype.prependWord = function(word) {

    this.words.splice(0, 0, word);
};

Command.prototype.appendWord = function(word) {

    this.words.push(word);
};

Command.prototype.numWordsLeft = function() {

    return this.words.length;
};

Command.prototype.hasWordsLeft = function() {

    return this.words.length > 0;
};

Command.prototype.assertWordsLeft = function(noneLeftText) {

    if (!this.hasWordsLeft()) {
        this.player.send(noneLeftText);
        return false;
    } else {
        return true;
    }
}

Command.prototype.peekWord = function() {

    return this.words[0];
}

Command.prototype.takeWord = function(pattern, options) {

    if (this.words.length > 0) {
        if (typeof pattern === "string") {
            pattern = new RegExp("^" + pattern + "$");
            if (options & Options.IfNotLast && this.words.length === 1) {
                return "";
            }
            if (pattern.test(this.words[0])) {
                return this.words.takeFirst();
            }
        } else {
            options = pattern;
            if (options & Options.IfNotLast && this.words.length === 1) {
                return "";
            }
            return this.words.takeFirst();
        }
    }

    return "";
};

Command.prototype.takeObject = function(pool) {

    var description = this.takeObjectsDescription();
    if (!description.name.isEmpty()) {
        var objects = this.objectsByDescription(description, pool);
        if (objects.length > 0) {
            return objects[0];
        }
    }

    return null;
};

Command.prototype.takeObjects = function(pool) {

    var description = this.takeObjectsDescription();
    if (!description.name.isEmpty()) {
        return this.objectsByDescription(description, pool);
    }

    return [];
}

Command.prototype.takeObjectsDescription = function(options) {

    this.takeWord("the", Options.IfNotLast);

    var description = { name: "", position: 0, containerName: "", containerPosition: 0 };

    if (this.words.isEmpty()) {
        return description;
    }

    description.name = this.words.takeFirst().toLower();
    description.position = 0;

    if (description.name.contains('.')) {
        var index = description.name.indexOf('.');
        description.position = description.name.left(index).toInt();
        if (description.position > 0) {
            description.name = description.name.mid(index + 1);
        }
    } else if (!this.words.isEmpty()) {
        description.position = Util.numericPosition(description.name);
        if (description.position > 0) {
            description.name = this.words.takeFirst().toLower();
        } else {
            description.position = this.words.first().toInt();
            if (description.position > 0) {
                this.words.removeFirst();
            }
        }
    }

    return description;
};

Command.prototype.takeRest = function() {

    var rest = this.words.join(" ");
    this.words.clear();
    return rest;
};

Command.prototype.objectByDescription = function(description, pool) {

    var objects = this.objectsByDescription(description, pool);
    if (!objects.isEmpty()) {
        return objects[0];
    }

    return null;
}

Command.prototype.objectsByDescription = function(description, pool) {

    var objects = [];
    if (description.name === "all") {
        objects = pool;
    } else {
        for (var i = 0, length = pool.length; i < length; i++) {
            var object = pool[i];
            var name;
            if (object.nameFromRoom) {
                name = object.nameFromRoom(this.currentRoom);
            } else {
                name = object.name;
            }
            var words = name.toLower().split(" ");
            for (var j = 0; j < words.length; j++) {
                var word = words[j];
                if (word.startsWith(description.name)) {
                    objects.push(object);
                    break;
                }
            }
        }
    }
    if (description.position > 0) {
        if (description.position <= objects.length) {
            var selected = objects[description.position - 1];
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

Command.prototype.send = function(message, arg1, arg2) {

    if (message.contains("%1")) {
        message = message.replace("%1", arg1);
        if (message.contains("%2")) {
            message = message.replace("%2", arg2);
        }
    }

    this.player.send(message);
};
