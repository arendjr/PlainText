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

var controller;

function Controller() {

    this.screen = document.getElementsByClassName("screen")[0];
    this.writeToScreen("Connecting...");

    var self = this;
    this.commandInput = document.getElementsByClassName("command-input")[0];
    this.commandInput.onkeypress = function(event) {
        if (event.keyCode === 13) {
            self.socket.send(self.commandInput.value);
            self.commandInput.value = "";
        }
    }

    this.socket = new WebSocket("ws://" + document.location.hostname + ":4801");
    this.socket.onopen = function(message) {
        self.writeToScreen("Connected.");
    };
    this.socket.onmessage = function(message) {
        self.writeToScreen(message.data);
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

    var element = document.createElement("div");
    if (message.substr(0, 5) === "\\x1B[" && message.substr(-7) === "\\x1B[0m") {
        var mIndex = message.indexOf("m");
        var color = message.substring(5, mIndex);
        if (color) {
            element.style.color = colorMap[color];
            message = message.substring(mIndex + 1, message.length - 7);
        }
    }

    var text = document.createTextNode(message);
    element.appendChild(text);
    this.screen.appendChild(element);

    this.screen.scrollTop = this.screen.scrollHeight;
}

function main() {

    controller = new Controller();
}
