/*global define:false, require:false, PT_WEBSOCKET_PORT:false*/
define(["lib/zepto"], function($) {

    "use strict";

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


    var screen;

    var player = {};
    var statusHeader;

    var commandInput;

    var commandListeners = [];
    var incomingMessageListeners = [];

    var history = [];
    var historyIndex = 0;
    var currentCommand = "";

    var pendingRequests = {};
    var requestId = 1;

    var socket;


    function init() {

        screen = $(".screen");
        writeToScreen("Connecting...");

        statusHeader = {
            "name": $(".status-header .name"),
            "hp": $(".status-header .hp"),
            "mp": $(".status-header .mp")
        };

        commandInput = $(".command-input");

        socket = new WebSocket("ws://" + document.location.hostname + ":" + PT_WEBSOCKET_PORT);

        correctScrollbars();

        attachListeners();

        setFocus();
    }

    function correctScrollbars() {

        var scrollDiv = document.createElement("div");
        scrollDiv.setAttribute("style", "width: 100px; height: 100px; " +
                                        "overflow: scroll; position: absolute; top: -9999px");
        document.body.appendChild(scrollDiv);

        var scrollbarWidth = scrollDiv.offsetWidth - scrollDiv.clientWidth;

        document.body.removeChild(scrollDiv);

        if (scrollbarWidth === 0) {
            return;
        }

        var styleSheets = document.styleSheets;
        for (var i = 0, length = styleSheets.length; i < length; i++) {
            var styleSheet = styleSheets[i];
            for (var j = 0, numRules = styleSheet.cssRules.length; j < numRules; j++) {
                var cssRule = styleSheet.cssRules[j];
                if (cssRule.selectorText === ".status-header, .screen, .command-input") {
                    cssRule.style.width = (640 + scrollbarWidth) + "px";
                    break;
                }
            }
        }

        $(".screen").css("overflow-y", "scroll");
    }

    function attachListeners() {

        commandInput.on("keypress", function(event) {
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

        commandInput.on("keydown", function(event) {
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

        screen.on("click", "a.go", function(event) {
            var exitName = event.target.text();
            sendCommand("go \"%1\"".arg(exitName));
        });

        socket.addEventListener("open", function(message) {
            writeToScreen("Connected.");
        }, false);

        socket.addEventListener("message", function(message) {
            if (message.data.startsWith("{") && message.data.endsWith("}")) {
                var data = JSON.parse(message.data);
                if (!data) {
                    writeToScreen("Error: Invalid JSON in reply: " + message.data);
                    return;
                }

                var requestId = data.requestId;

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
                        require(["admin"]);
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
        }, false);

        socket.addEventListener("close", function() {
            writeToScreen("Connection closed.");
        }, false);

        socket.addEventListener("error", function() {
            writeToScreen("Connection error.");
        }, false);
    }

    function addStyle(fileName) {

        if (!fileName.endsWith(".css")) {
            fileName += ".css";
        }

        if ($("link[href='%1']".arg(fileName), document.head).length === 0) {
            $("<link>", {
                "rel": "stylesheet",
                "href": fileName
            }).appendTo(document.head);
        }
    }

    function addCommandListener(listener) {

        commandListeners.insert(listener);
    }

    function removeCommandListener(listener) {

        commandListeners.removeAll(listener);
    }

    function notifyCommandListeners(command) {

        commandListeners.forEach(function(listener) {
            var result = listener(command);
            if (result !== undefined) {
                command = result;
            }
        });
        return command;
    }

    function addIncomingMessageListener(listener) {

        incomingMessageListeners.insert(listener);
    }

    function removeIncomingMessageListener(listener) {

        incomingMessageListeners.removeAll(listener);
    }

    function notifyIncomingMessageListeners(message) {

        incomingMessageListeners.forEach(function(listener) {
            listener(message);
        });
    }

    function writeToScreen(message) {

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

        if (screen.children().length >= 500) {
            screen.children().first().remove();
        }

        screen.append(div);

        screen[0].scrollTop = screen[0].scrollHeight;
    }

    function sendCommand(command) {

        socket.send(command);
    }

    function sendApiCall(command, callback, errorCallback) {

        var id = "request" + requestId;
        if (callback) {
            pendingRequests[id] = {
                "success": callback,
                "error": errorCallback
            }
        }

        var parts = command.split(" ");
        parts.splice(1, 0, id);
        socket.send("api-" + parts.join(" "));

        requestId++;
    }

    function setFocus() {

        commandInput[0].focus();
    }

    init();

    return {
        "addStyle": addStyle,
        "addCommandListener": addCommandListener,
        "removeCommandListener": removeCommandListener,
        "addIncomingMessageListener": addIncomingMessageListener,
        "removeIncomingMessageListener": removeIncomingMessageListener,
        "writeToScreen": writeToScreen,
        "sendCommand": sendCommand,
        "sendApiCall": sendApiCall,
        "setFocus": setFocus
    };
});
