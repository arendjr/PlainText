const colorMap = {
    "37;1": "#ffffff",
    37: "#c0c0c0",
    "30;1": "#808080",
    30: "#000000",
    "31;1": "#ff0000",
    31: "#800000",
    "33;1": "#ffff00",
    33: "#808000",
    "32;1": "#00ff00",
    32: "#008000",
    "36;1": "#00ffff",
    36: "#008080",
    "34;1": "#0000ff",
    34: "#000080",
    "35;1": "#ff00ff",
    35: "#800080",
};

const keys = {
    KEY_BACKSPACE: 8,
    KEY_TAB: 9,
    KEY_RETURN: 13,
    KEY_ESC: 27,
    KEY_SPACE: 32,
    KEY_LEFT: 37,
    KEY_UP: 38,
    KEY_RIGHT: 39,
    KEY_DOWN: 40,
    KEY_DELETE: 46,
    KEY_HOME: 36,
    KEY_END: 35,
    KEY_PAGEUP: 33,
    KEY_PAGEDOWN: 34,
    KEY_INSERT: 45,
};

let player = {};

const commandListeners = new Set();
const incomingMessageListeners = new Set();

const history = [];
let historyIndex = 0;
let currentCommand = "";

const pendingRequests = {};
let requestId = 1;

const screen = $(".screen");
writeToScreen("Connecting...");

const statusHeader = {
    name: $(".status-header .name"),
    hp: $(".status-header .hp"),
    mp: $(".status-header .mp"),
};

const commandInput = $(".command-input");

const socket = new WebSocket("ws://" + document.location.host);

function correctScrollbars() {
    const scrollDiv = document.createElement("div");
    scrollDiv.setAttribute(
        "style",
        "width: 100px; height: 100px; " +
            "overflow: scroll; position: absolute; top: -9999px"
    );
    document.body.appendChild(scrollDiv);

    const scrollbarWidth = scrollDiv.offsetWidth - scrollDiv.clientWidth;

    document.body.removeChild(scrollDiv);

    if (scrollbarWidth === 0) {
        return;
    }

    const cssRule = Array.prototype.flatMap
        .call(document.styleSheets, styleSheet => styleSheet.cssRules)
        .find(
            cssRule =>
                cssRule.selectorText ===
                ".status-header, .screen, .command-input"
        );
    if (cssRule) {
        cssRule.style.width = 640 + scrollbarWidth + "px";
    }

    screen.css("overflow-y", "scroll");
}

function attachListeners() {
    commandInput.on("keypress", event => {
        if (event.keyCode === keys.KEY_RETURN) {
            var command = commandInput.val();

            if (player.name) {
                if (history[history.length - 1] !== command) {
                    history.push(command);
                }
                historyIndex = 0;
            }

            command = notifyCommandListeners(command);
            if (command !== false) {
                sendCommand(command);
            }

            commandInput.val("");
        }
    });

    commandInput.on("keydown", event => {
        if (event.keyCode === keys.KEY_UP) {
            if (historyIndex === 0) {
                currentCommand = commandInput.val();
            }
            historyIndex++;
            if (historyIndex > history.length) {
                historyIndex = history.length;
            } else {
                commandInput.val(history[history.length - historyIndex]);
            }
        } else if (event.keyCode === keys.KEY_DOWN) {
            historyIndex--;
            if (historyIndex < 0) {
                historyIndex = 0;
            } else {
                if (historyIndex === 0) {
                    commandInput.val(currentCommand);
                } else {
                    commandInput.val(history[history.length - historyIndex]);
                }
            }
        }
    });

    screen.on("click", "a.go", event => {
        const exitName = event.target.text();
        sendCommand(`go "${exitName}"`);
    });

    socket.addEventListener("open", () => writeToScreen("Connected."));

    socket.addEventListener("message", message => {
        if (message.data.startsWith("{") && message.data.endsWith("}")) {
            const data = JSON.parse(message.data);
            if (!data) {
                writeToScreen("Error: Invalid JSON in reply: " + message.data);
                return;
            }

            const { requestId } = data;

            if (pendingRequests.hasOwnProperty(requestId)) {
                if (data.errorCode === 0) {
                    pendingRequests[requestId].success(data.data);
                } else {
                    if (pendingRequests[requestId].error) {
                        pendingRequests[requestId].error();
                    }
                    writeToScreen("Error: " + data.errorMessage);
                }

                delete pendingRequests[requestId];
            } else if (data.player) {
                if (!player.isAdmin && data.player.isAdmin) {
                    import("./admin.js");
                }

                player = data.player;

                statusHeader.name.text(player.name);

                statusHeader.hp.text(player.hp + "HP");
                if (player.hp < player.maxHp / 4) {
                    statusHeader.hp.css("color", "#f00");
                } else {
                    statusHeader.hp.css("color", "");
                }

                statusHeader.mp.text(player.mp + "MP");
                if (player.mp < player.maxMp / 4) {
                    statusHeader.mp.css("color", "#f00");
                } else {
                    statusHeader.mp.css("color", "");
                }
            } else if (data.inputType) {
                commandInput.attr("type", data.inputType);
            }
        } else {
            notifyIncomingMessageListeners(message.data);

            writeToScreen(message.data);
        }
    });

    socket.addEventListener("close", () => writeToScreen("Connection closed."));
    socket.addEventListener("error", () => writeToScreen("Connection error."));
}

export function addStyle(fileName) {
    if (!fileName.endsWith(".css")) {
        fileName += ".css";
    }

    if ($(`link[href='${fileName}']`, document.head).length === 0) {
        $("<link>", {
            rel: "stylesheet",
            href: fileName,
        }).appendTo(document.head);
    }
}

export function addCommandListener(listener) {
    commandListeners.add(listener);
}

export function removeCommandListener(listener) {
    commandListeners.delete(listener);
}

function notifyCommandListeners(command) {
    for (const listener of commandListeners) {
        var result = listener(command);
        if (result !== undefined) {
            command = result;
        }
    }
    return command;
}

export function addIncomingMessageListener(listener) {
    incomingMessageListeners.add(listener);
}

export function removeIncomingMessageListener(listener) {
    incomingMessageListeners.delete(listener);
}

function notifyIncomingMessageListeners(message) {
    for (const listener of incomingMessageListeners) {
        listener(message);
    }
}

export function writeToScreen(message) {
    if (message.trim().length === 0) {
        return;
    }

    const div = document.createElement("div");

    let index;
    while ((index = message.indexOf("\x1B[")) > -1) {
        const mIndex = message.indexOf("m", index);
        const endIndex = message.indexOf("\x1B[0m", mIndex);

        if (index > 0) {
            div.appendChild(document.createTextNode(message.substr(0, index)));
        }

        const color = message.substring(index + 2, mIndex);

        const span = document.createElement("span");
        span.appendChild(
            document.createTextNode(message.substring(mIndex + 1, endIndex))
        );
        span.style.color = colorMap[color];
        div.appendChild(span);

        message = message.substr(endIndex + 4);
    }
    if (message.length > 0) {
        div.appendChild(document.createTextNode(message));
    }

    if (screen.children().length >= 500) {
        screen.children().first().remove();
    }

    screen.append(div);

    screen[0].scrollTop = screen[0].scrollHeight;
}

export function sendCommand(command) {
    socket.send(command);
}

export function sendApiCall(command, callback, errorCallback) {
    const id = "request" + requestId;
    if (callback) {
        pendingRequests[id] = {
            success: callback,
            error: errorCallback,
        };
    }

    const parts = command.split(" ");
    parts.splice(1, 0, id);
    socket.send("api-" + parts.join(" "));

    requestId++;
}

export function setFocus() {
    commandInput[0].focus();
}

correctScrollbars();
attachListeners();
setFocus();
