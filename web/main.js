export const Keys = {
    RETURN: 13,
    LEFT_ARROW: 37,
    UP_ARROW: 38,
    RIGHT_ARROW: 39,
    DOWN_ARROW: 40,
};

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

let player = {};

const commandListeners = new Set();
const incomingMessageListeners = new Set();

const history = [];
let historyIndex = 0;
let currentCommand = "";

const pendingRequests = new Map();
let requestId = 1;

const screen = document.querySelector(".screen");
writeToScreen("Connecting...");

const statusHeader = {
    name: document.querySelector(".status-header .name"),
    hp: document.querySelector(".status-header .hp"),
    mp: document.querySelector(".status-header .mp"),
};

const commandInput = document.querySelector(".command-input");

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

    screen.style.overflowY = "scroll";
}

function attachListeners() {
    commandInput.addEventListener("keypress", event => {
        if (event.keyCode === Keys.RETURN) {
            var command = commandInput.value;

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

            commandInput.value = "";
        }
    });

    commandInput.addEventListener("keydown", event => {
        if (event.keyCode === Keys.UP_ARROW) {
            if (historyIndex === 0) {
                currentCommand = commandInput.value;
            }
            historyIndex++;
            if (historyIndex > history.length) {
                historyIndex = history.length;
            } else {
                commandInput.value = history[history.length - historyIndex];
            }
        } else if (event.keyCode === Keys.DOWN_ARROW) {
            historyIndex--;
            if (historyIndex < 0) {
                historyIndex = 0;
            } else if (historyIndex === 0) {
                commandInput.value = currentCommand;
            } else {
                commandInput.value = history[history.length - historyIndex];
            }
        }
    });

    screen.addEventListener("click", event => {
        const { target } = event;
        if (target.tagName === "A" && target.classList.contains("go")) {
            const exitName = target.textContent;
            sendCommand(`go "${exitName}"`);
        }
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

            const resolveRequest = pendingRequests.get(requestId);
            if (resolveRequest) {
                if (data.errorCode === 0) {
                    resolveRequest(data.data);
                } else {
                    resolveRequest(Promise.reject(data.errorCode));
                    writeToScreen("Error: " + data.errorMessage);
                }

                pendingRequests.delete(requestId);
            } else if (data.player) {
                if (!player.isAdmin && data.player.isAdmin) {
                    import("./admin.js");
                }

                player = data.player;

                statusHeader.name.textContent = player.name;

                statusHeader.hp.textContent = player.hp + "HP";
                const healthIsCritical = player.hp < player.maxHp / 4;
                statusHeader.hp.style.color = healthIsCritical ? "#f00" : "";

                statusHeader.mp.textContent = player.mp + "MP";
                const mpIsLow = player.mp < player.maxMp / 4;
                statusHeader.mp.style.color = mpIsLow ? "#f00" : "";
            } else if (data.inputType) {
                commandInput.setAttribute("type", data.inputType);
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
    if (!document.head.querySelector(`link[href='${fileName}']`)) {
        const link = document.createElement("link");
        link.setAttribute("rel", "stylesheet");
        link.setAttribute("href", fileName);
        document.head.appendChild(link);
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
            div.appendChild(document.createTextNode(message.slice(0, index)));
        }

        const color = message.slice(index + 2, mIndex);

        const span = document.createElement("span");
        span.appendChild(
            document.createTextNode(message.slice(mIndex + 1, endIndex))
        );
        span.style.color = colorMap[color];
        div.appendChild(span);

        message = message.slice(endIndex + 4);
    }
    if (message.length > 0) {
        div.appendChild(document.createTextNode(message));
    }

    if (screen.children.length >= 500) {
        screen.firstChild.remove();
    }

    screen.appendChild(div);

    screen.scrollTop = screen.scrollHeight;
}

export function sendCommand(command) {
    socket.send(command);
}

export function sendApiCall(command) {
    const id = "request" + requestId;
    const promise = new Promise(resolve => pendingRequests.set(id, resolve));

    const parts = command.split(" ");
    parts.splice(1, 0, id);
    socket.send("api-" + parts.join(" "));

    requestId++;

    return promise;
}

export function setFocus() {
    commandInput.focus();
}

window.addEventListener("DOMContentLoaded", () => {
    correctScrollbars();
    attachListeners();
    setFocus();
});
