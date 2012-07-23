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

String.prototype.contains = function(string) {

    return this.indexOf(string) > -1;
};

String.prototype.startsWith = function(string) {

    return this.substr(0, string.length) === string;
};

String.prototype.trimmed = function() {

    return this.replace(/^\s+|\s+$/g, "");
};

function loadScript(fileName) {

    var script = document.createElement("script");
    script.setAttribute("type", "text/javascript");
    script.setAttribute("src", fileName);
    document.head.appendChild(script);
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

function element(selector) {

    return document.querySelector(selector);
}

function elements(selector) {

    return document.querySelectorAll(selector);
}

var isPhoneGap = (getQueryParam("phonegap") === "true");
var isIPad = navigator.userAgent.toLowerCase().contains("ipad");
var isIPhone = navigator.userAgent.toLowerCase().contains("iphone");

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

    this.history = [];
    this.historyIndex = 0;
    this.currentCommand = "";

    this.editScreen = element(".edit-screen");
    this.editField = element("#edit-field");
    this.setCommand = "";
    this.onMessageHook = null;

    var self = this;
    this.commandInput = element(".command-input");
    this.commandInput.onkeypress = function(event) {
        if (event.keyCode === keys.KEY_RETURN) {
            var command = self.commandInput.value;

            self.socket.send(command);
            self.commandInput.value = "";

            if (self.player.name) {
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
            if (data && data.player) {
                if (!self.player.isAdmin && data.player.isAdmin) {
                    loadScript("admin.js");
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

    window.onorientationchange = function() {
        self.updateLayout();
    };
    this.commandInput.onfocus = function() {
        self.updateLayout();
        self.screen.scrollTop = self.screen.scrollHeight;
    }
    this.commandInput.onblur = function() {
        self.updateLayout();
    }

    if (isPhoneGap) {
        element(".status-header").style.backgroundColor = "#c0c0c0";
        this.updateLayout();
    } else if (document.body.clientWidth > 660 && !isIPad) {
        this.setFocus();
    }
}

Controller.prototype.writeToScreen = function(message) {

    if (message.trimmed().length === 0) {
        return;
    }

    var containsExits = message.contains("Obvious exits:");

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

        if (containsExits) {
            var regExp = /[:,] ([\w '-]+)/g;
            var startIndex = mIndex + 1;
            var array;

            regExp.lastIndex = startIndex;
            while ((array = regExp.exec(message)) !== null) {
                if (array.index >= endIndex) {
                    break;
                }

                span.appendChild(document.createTextNode(message.substring(startIndex, array.index + 2)));

                var anchor = document.createElement("a");
                anchor.className = "go";
                anchor.setAttribute("href", "javascript:void(0)");
                anchor.textContent = array[1];
                span.appendChild(anchor);

                startIndex = regExp.lastIndex;
            }

            if (startIndex < endIndex) {
                span.appendChild(document.createTextNode(message.substring(startIndex, endIndex)));
            }
        } else {
            span.appendChild(document.createTextNode(message.substring(mIndex + 1, endIndex)));
        }

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

Controller.prototype.updateLayout = function() {

    var body = document.body;

    var width = body.clientWidth;
    var height = body.clientHeight;

    if (width > 660) {
        body.scrollTop = 0;
        return;
    }

    var defaultScreenHeight = (isPhoneGap ? 424 : 320);

    body.scrollTop = 2000;
    var actualScrollTop = body.scrollTop;

    var keyboardShown = (actualScrollTop > 0);
    if (keyboardShown) {
        if (isPhoneGap) {
            actualScrollTop -= 44;
        }

        this.screen.style.height = (defaultScreenHeight - actualScrollTop) + "px";
        body.scrollTop = 0;
    } else {
        this.screen.style.height = defaultScreenHeight + "px";
    }
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

    if (isIPad || isIPhone) {
        loadScript("tappable.js");

        function initTappable() {

            if (!window.tappable) {
                setTimeout(initTappable, 50);
                return;
            }

            tappable(".go", function(event) {
                var exitName = event.target.textContent;
                controller.socket.send("go \"" + exitName + "\"");
            });
        }

        setTimeout(initTappable, 50);
    } else {
        controller.screen.addEventListener("click", function(event) {
            var target = event.target;
            if (target.className === "go") {
                var exitName = target.textContent;
                controller.socket.send("go \"" + exitName + "\"");
            }
        }, false);
    }
}
