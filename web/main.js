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
}

var controller;

function Controller() {

    this.screen = document.getElementsByClassName("screen")[0];
    this.writeToScreen("Connecting...");

    this.character = {};
    this.statusHeader = {
        "name": document.querySelector(".status-header .name"),
        "hp": document.querySelector(".status-header .hp"),
    };

    this.history = [];
    this.historyIndex = 0;
    this.currentCommand = '';

    var self = this;
    this.commandInput = document.getElementsByClassName("command-input")[0];
    this.commandInput.onkeypress = function(event) {
        if (event.keyCode === keys.KEY_RETURN) {
            var command = self.commandInput.value;
            self.socket.send(command);
            self.commandInput.value = "";

            if (self.character.name) {
                if (self.history[self.history.length - 1] !== command) {
                    self.history.push(command);
                }
                self.historyIndex = 0;
            }
        }
    }
    this.commandInput.onkeydown = function(event) {
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
    }

    this.socket = new WebSocket("ws://" + document.location.hostname + ":4802");
    this.socket.onopen = function(message) {
        self.writeToScreen("Connected.");
    };
    this.socket.onmessage = function(message) {
        if (message.data.substr(0, 1) === "{" && message.data.substr(-1) === "}") {
            var data = JSON.parse(message.data);
            if (data.character) {
                self.character = data.character;

                if (self.character.isAdmin) {
                    self.commandInput.removeAttribute("maxlength");
                }

                self.statusHeader.name.innerText = self.character.name;
                self.statusHeader.hp.innerText = self.character.hp + "HP";
            }
        } else {
            self.writeToScreen(message.data);
        }
    };
    this.socket.onclose = function(message) {
        self.writeToScreen("Connection closed.");
    };
    this.socket.onerror = function() {
        self.writeToScreen("Connection error.");
    };

    this.commandInput.focus();
}

Controller.prototype.writeToScreen = function(message) {

    var div = document.createElement("div");

    var index;
    while ((index = message.indexOf("\x1B[", index)) > -1) {
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

    this.screen.appendChild(div);

    this.screen.scrollTop = this.screen.scrollHeight;
}

function main() {

    controller = new Controller();
}
