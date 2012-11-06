var colorMap = {
    "37;1": "#ffffff", "37": "#c0c0c0",
    "30;1": "#808080", "30": "#000000",
    "31;1": "#ff0000", "31": "#800000",
    "33;1": "#ffff00", "33": "#808000",
    "32;1": "#00ff00", "32": "#008000",
    "36;1": "#00ffff", "36": "#008080",
    "34;1": "#0000ff", "34": "#000080",
    "35;1": "#ff00ff", "35": "#800080"
};

var keys = {
    KEY_BACKSPACE: 8,
    KEY_TAB:       9,
    KEY_RETURN:   13,
    KEY_ESC:      27,
    KEY_SPACE:    32,
    KEY_LEFT:     37,
    KEY_UP:       38,
    KEY_RIGHT:    39,
    KEY_DOWN:     40,
    KEY_DELETE:   46,
    KEY_HOME:     36,
    KEY_END:      35,
    KEY_PAGEUP:   33,
    KEY_PAGEDOWN: 34,
    KEY_INSERT:   45
};

var scriptCallbacks = {};


Array.prototype.append = function(value) {

    return this.push(value);
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


Element.prototype.hide = function() {

    this.style.display = "none";
};

Element.prototype.show = function() {

    this.style.display = "block";
};


Object.defineProperty(Object.prototype, "contains", { value: Object.prototype.hasOwnProperty });


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


var Util = {};

Util.directions = [
    { "name": "north", "opposite": "south" },
    { "name": "northeast", "opposite": "southwest" },
    { "name": "east", "opposite": "west" },
    { "name": "southeast", "opposite": "northwest" },
    { "name": "south", "opposite": "north" },
    { "name": "southwest", "opposite": "northeast" },
    { "name": "west", "opposite": "east" },
    { "name": "northwest", "opposite": "southeast" },
    { "name": "up", "opposite": "down" },
    { "name": "down", "opposite": "up" }
];

Util.indexOfDirection = function(direction) {

    for (var i = 0; i < 10; i++) {
        if (this.directions[i].name === direction) {
            return i;
        }
    }

    return -1;
};

Util.isDirection = function(string) {

    return this.indexOfDirection(string) > -1;
};

Util.opposingDirection = function(direction) {

    return this.directions[this.indexOfDirection(direction)].opposite;
};


function bound(min, value, max) {

    return (value < min) ? min : ((value > max) ? max : value);
}

function element(selector) {

    return document.querySelector(selector);
}

function elements(selector) {

    var nodeList = document.querySelectorAll(selector);
    var array = [];
    for (var i = 0; i < nodeList.length; i++) {
        array.push(nodeList[i]);
    }
    return array;
}

function getQueryParam(name, defaultValue) {

    var params = window.location.search.substr(1).split("&");
    for (var i = 0; i < params.length; i++) {
        var parts = params[i].split("=");
        if (parts.length !== 2) {
            continue;
        }
        if (parts[0] === name) {
            return decodeURIComponent(parts[1]);
        }
    }
    return defaultValue;
}

function loadScript(fileName, callback) {

    var script = document.createElement("script");
    script.setAttribute("type", "text/javascript");
    script.setAttribute("src", fileName);
    document.head.appendChild(script);

    if (callback) {
        var index = fileName.lastIndexOf("/");
        if (index === -1) {
            scriptCallbacks[fileName] = callback;
        } else {
            scriptCallbacks[fileName.substr(index + 1)] = callback;
        }
    }
}

function loadStyle(fileName) {

    var link = document.createElement("link");
    link.setAttribute("rel", "stylesheet");
    link.setAttribute("href", fileName);
    document.head.appendChild(link);
}

function scriptLoaded(fileName) {

    if (scriptCallbacks.contains(fileName)) {
        scriptCallbacks[fileName]();
    }
}


var controller;

function Controller() {

    this.screen = element(".screen");
    this.writeToScreen("Connecting...");

    this.player = {};
    this.statusHeader = {
        "name": element(".status-header .name"),
        "hp": element(".status-header .hp"),
        "mp": element(".status-header .mp"),
    };

    this.commandInput = element(".command-input");

    this.commandListeners = [];
    this.incomingMessageListeners = [];

    this.history = [];
    this.historyIndex = 0;
    this.currentCommand = "";

    this.pendingRequests = {};
    this.requestId = 1;

    this.socket = new WebSocket("ws://" + document.location.hostname + ":4802");

    this.attachListeners();

    this.setFocus();
}

Controller.prototype.attachListeners = function() {

    var self = this;

    this.commandInput.addEventListener("keypress", function(event) {
        if (event.keyCode === keys.KEY_RETURN) {
            var command = self.commandInput.value;

            if (self.player.name) {
                if (self.history[self.history.length - 1] !== command) {
                    self.history.push(command);
                }
                self.historyIndex = 0;
            }

            command = self.notifyCommandListeners(command);
            if (command !== false) {
                self.sendCommand(command);
            }

            self.commandInput.value = "";
        }
    }, false);

    this.commandInput.addEventListener("keydown", function(event) {
        if (event.keyCode === keys.KEY_UP) {
            if (self.historyIndex === 0) {
                self.currentCommand = self.commandInput.value;
            }
            self.historyIndex++;
            if (self.historyIndex > self.history.length) {
                self.historyIndex = self.history.length;
            } else {
                self.commandInput.value = self.history[self.history.length - self.historyIndex];
            }
        } else if (event.keyCode === keys.KEY_DOWN) {
            self.historyIndex--;
            if (self.historyIndex < 0) {
                self.historyIndex = 0;
            } else {
                if (self.historyIndex === 0) {
                    self.commandInput.value = self.currentCommand;
                } else {
                    self.commandInput.value = self.history[self.history.length - self.historyIndex];
                }
            }
        }
    }, false);

    this.socket.addEventListener("open", function(message) {
        self.writeToScreen("Connected.");
    }, false);

    this.socket.addEventListener("message", function(message) {
        if (message.data.substr(0, 1) === "{" && message.data.substr(-1) === "}") {
            var data = JSON.parse(message.data);
            if (!data) {
                return;
            }

            var requestId = data.requestId;

            if (self.pendingRequests.hasOwnProperty(requestId)) {
                if (data.errorCode === 0) {
                    self.pendingRequests[requestId](data.data);
                } else {
                    self.writeToScreen("Error: " + data.errorMessage);
                }

                delete self.pendingRequests[requestId];
            } else if (data.player) {
                if (!self.player.isAdmin && data.player.isAdmin) {
                    loadScript("admin/admin.js");
                }

                self.player = data.player;

                self.statusHeader.name.textContent = self.player.name;

                self.statusHeader.hp.textContent = self.player.hp + "HP";
                if (self.player.hp < self.player.maxHp / 4) {
                    self.statusHeader.hp.style.color = "#f00";
                } else {
                    self.statusHeader.hp.style.color = "";
                }

                self.statusHeader.mp.textContent = self.player.mp + "MP";
                if (self.player.mp < self.player.maxMp / 4) {
                    self.statusHeader.mp.style.color = "#f00";
                } else {
                    self.statusHeader.mp.style.color = "";
                }
            }
        } else {
            self.notifyIncomingMessageListeners(message.data);

            self.writeToScreen(message.data);
        }
    }, false);

    this.socket.addEventListener("close", function() {
        self.writeToScreen("Connection closed.");
    }, false);

    this.socket.addEventListener("error", function() {
        self.writeToScreen("Connection error.");
    }, false);
}

Controller.prototype.addCommandListener = function(listener) {

    this.commandListeners.insert(listener);
};

Controller.prototype.removeCommandListener = function(listener) {

    this.commandListeners.removeAll(listener);
};

Controller.prototype.notifyCommandListeners = function(command) {

    this.commandListeners.forEach(function(listener) {
        var result = listener(command);
        if (result !== undefined) {
            command = result;
        }
    });
    return command;
};

Controller.prototype.addIncomingMessageListener = function(listener) {

    this.incomingMessageListeners.insert(listener);
};

Controller.prototype.removeIncomingMessageListener = function(listener) {

    this.incomingMessageListeners.removeAll(listener);
};

Controller.prototype.notifyIncomingMessageListeners = function(message) {

    this.incomingMessageListeners.forEach(function(listener) {
        listener(message);
    });
};

Controller.prototype.writeToScreen = function(message) {

    if (message.trimmed().length === 0) {
        return;
    }

    var div = document.createElement("div");

    var index;
    while ((index = message.indexOf("\x1B[")) > -1) {
        var mIndex = message.indexOf("m", index);
        var endIndex = message.indexOf("\x1B[0m", mIndex);

        if (index > 0) {
            div.appendChild(document.createTextNode(message.substr(0, index)));
        }

        var color = message.substring(index + 2, mIndex);

        var span = document.createElement("span");
        span.appendChild(document.createTextNode(message.substring(mIndex + 1, endIndex)));
        span.style.color = colorMap[color];
        div.appendChild(span);

        message = message.substr(endIndex + 4);
    }
    if (message.length > 0) {
        div.appendChild(document.createTextNode(message));
    }

    if (this.screen.childNodes.length >= 500) {
        this.screen.removeChild(this.screen.childNodes[0]);
    }

    this.screen.appendChild(div);

    this.screen.scrollTop = this.screen.scrollHeight;
}

Controller.prototype.sendCommand = function(command) {

    this.socket.send(command);
}

Controller.prototype.sendApiCall = function(command, callback) {

    var requestId = "request" + this.requestId;
    if (callback) {
        this.pendingRequests[requestId] = callback;
    }

    var parts = command.split(" ");
    parts.splice(1, 0, requestId);
    this.socket.send("api-" + parts.join(" "));

    this.requestId++;
}

Controller.prototype.setFocus = function() {

    this.commandInput.focus();
}


function main() {

    controller = new Controller();

    if ((window.notifications && notifications.requestPermission) ||
        (window.webkitNotifications && webkitNotifications.requestPermission)) {
        loadScript("notifications.js");
    }

    controller.screen.addEventListener("click", function(event) {
        var target = event.target;
        if (target.className === "go") {
            var exitName = target.textContent;
            controller.sendCommand("go \"" + exitName + "\"");
        }
    }, false);
}
