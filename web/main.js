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

if (window.webkitNotifications) {
    window.notifications = window.webkitNotifications;
    window.notifications.permissionRequested = false;
}

window.onfocus = function() {
    window.isActive = true;
};
window.onblur = function() {
    window.isActive = false;
};
window.onfocus();

String.prototype.trimmed = function() {

    return this.replace(/^\s+|\s+$/g, "");
};

function prettyPrint(code) {

    var indent = "";
    var fromIndex = 0;
    do {
        var firstSemicolon = code.indexOf(";", fromIndex);
        var firstOpeningBrace = code.indexOf("{", fromIndex);
        var firstIndentedClosingBrace = code.indexOf("    }", fromIndex);
        var firstClosingBrace = code.indexOf("}", fromIndex);

        if (firstSemicolon === -1) {
            firstSemicolon = 99999;
        }
        if (firstOpeningBrace === -1) {
            firstOpeningBrace = 99999;
        }
        if (firstIndentedClosingBrace === -1) {
            firstIndentedClosingBrace = 99999;
        }
        if (firstClosingBrace === -1) {
            firstClosingBrace = 99999;
        }

        if (firstSemicolon < firstOpeningBrace && firstSemicolon < firstClosingBrace) {
            code = code.substr(0, firstSemicolon + 1) + "\n" +
                   indent + code.substr(firstSemicolon + 1).trimmed();
            fromIndex = firstSemicolon + 1;
        } else if (firstOpeningBrace < firstClosingBrace) {
            indent += "    ";
            code = code.substr(0, firstOpeningBrace + 1) + "\n" +
                   indent + code.substr(firstOpeningBrace + 1).trimmed();
            fromIndex = firstOpeningBrace + 1;
        } else if (firstIndentedClosingBrace < firstClosingBrace) {
            indent = indent.substr(0, indent.length - 4);
            code = code.substr(0, firstIndentedClosingBrace) + "}\n" +
                   indent + code.substr(firstIndentedClosingBrace + 5).trimmed();
            fromIndex = firstIndentedClosingBrace + 2;
        } else {
            indent = indent.substr(0, indent.length - 4);
            code = code.substr(0, firstClosingBrace + 1) + "\n" +
                   indent + code.substr(firstClosingBrace + 1).trimmed();
            fromIndex = firstClosingBrace + 1;
        }
    } while (firstSemicolon < 99999 ||
             firstOpeningBrace < 99999 ||
             firstClosingBrace < 99999);

    code = code.replace(/}\s+(else|while)/g, "} $1");
    code = code.replace(/}\s+(\)|,)/g, "}$1");

    return code;
}

function Controller() {

    this.screen = document.getElementsByClassName("screen")[0];
    this.writeToScreen("Connecting...");

    this.player = {};
    this.statusHeader = {
        "name": document.querySelector(".status-header .name"),
        "hp": document.querySelector(".status-header .hp"),
    };

    this.history = [];
    this.historyIndex = 0;
    this.currentCommand = "";

    this.editScreen = document.querySelector(".edit-screen");
    this.editField = document.getElementById("edit-field");
    this.setCommand = "";
    this.onMessageHook = null;

    var self = this;
    this.commandInput = document.getElementsByClassName("command-input")[0];
    this.commandInput.onkeypress = function(event) {
        if (event.keyCode === keys.KEY_RETURN) {
            var command = self.commandInput.value;

            if (self.player.isAdmin) {
                if (command.substr(0, 10) === "edit-prop " ||
                    command.substr(0, 13) === "edit-trigger ") {

                    self.setCommand = "set" + command.substr(4);
                    command = "get" + command.substr(4);

                    self.editScreen.style.display = "block";

                    self.onMessageHook = function(message) {
                        message = message.trimmed();
                        if (message.substr(0, 9) === "(function" &&
                            message.substr(-2) === "})") {
                            message = prettyPrint(message);
                        }

                        self.editField.value = message;
                        self.editField.focus();
                    }
                }
            }

            self.socket.send(command);
            self.commandInput.value = "";

            if (self.player.name) {
                if (self.history[self.history.length - 1] !== command) {
                    self.history.push(command);
                }
                self.historyIndex = 0;
            }

            if (window.notifications &&
                notifications.requestPermission &&
                !notifications.permissionRequested) {
                notifications.requestPermission(function() {});
                notifications.permissionRequested = true;
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
            if (data.player) {
                self.player = data.player;

                if (self.player.isAdmin) {
                    self.commandInput.removeAttribute("maxlength");

                    document.getElementById("edit-cancel-button").onclick = function() {
                        self.editScreen.style.display = "none";
                        self.commandInput.focus();
                    }
                    document.getElementById("edit-submit-button").onclick = function() {
                        self.socket.send(self.setCommand + " " + self.editField.value);

                        self.editScreen.style.display = "none";
                        self.commandInput.focus();
                    }
                    self.editField.onkeydown = function(event) {
                        if (event.keyCode === keys.KEY_TAB) {
                            var value = self.editField.value;
                            var start = self.editField.selectionStart;
                            var end = self.editField.selectionEnd;
                            self.editField.value = value.substr(0, start) + "    " + value.substr(end);
                            self.editField.selectionStart = start + 4;
                            self.editField.selectionEnd = start + 4;
                            return false;
                        }
                    }
                }

                self.statusHeader.name.innerText = self.player.name;
                self.statusHeader.hp.innerText = self.player.hp + "HP";
            }
        } else if (self.onMessageHook) {
            self.onMessageHook(message.data);
            self.onMessageHook = null;
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

    if (!window.isActive && window.notifications &&
        notifications.checkPermission &&
        notifications.checkPermission() === 0 &&
        /^\w+ (says|tells|shouts)/.test(message)) {
        var notification = notifications.createNotification(null, "MUD", message);
        notification.show()

        setTimeout(function(){
            notification.cancel();
        }, 10000);
    }

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
