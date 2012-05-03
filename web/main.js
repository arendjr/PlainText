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

    this.socket = new WebSocket("ws://localhost:4801");
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
}

Controller.prototype.writeToScreen = function(message) {

    var element = document.createElement("div");
    var text = document.createTextNode(message);
    element.appendChild(text);
    this.screen.appendChild(element);
}

function main() {

    controller = new Controller();
}
