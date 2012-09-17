(function() {

    var self = controller;

    var propertyEditor = element(".property-editor");

    function loadPropertyEditor() {
        loadScript("codemirror/codemirror.js");
        loadStyle("codemirror/codemirror.css");

        propertyEditor.show = function(value) {
            if (value && value.trimmed().startsWith("(function")) {
                this.editor.setOption("mode", "javascript");
                this.editor.setOption("lineWrapping", false);
            } else {
                this.editor.setOption("mode", "null");
                this.editor.setOption("lineWrapping", true);
            }

            this.style.display = "block";
            this.editor.setValue(value !== undefined ? value.trimmed() : "");
            this.editor.focus();
        };
        propertyEditor.hide = function() {
            this.style.display = "none";
            self.commandInput.focus();
        };

        element("#edit-cancel-button").onclick = function() {
            propertyEditor.hide();
        };
        element("#edit-submit-button").onclick = function() {
            var value = propertyEditor.editor.getValue();
            if (saveCommand.startsWith("set-prop")) {
                value = value.replace(/\n/g, "\\n");
            } else if (saveCommand.startsWith("exec-script")) {
                value = value.replace(/\s+/g, " ");
            }

            if (saveCommand.startsWith("api-")) {
                self.sendApiCall("save1", saveCommand.substr(4) + " " + value, function(data) {
                    self.writeToScreen(data);

                    propertyEditor.hide();
                });
            } else {
                self.socket.send(saveCommand + " " + value);

                propertyEditor.hide();
            }
        };

        function initCodeMirror() {
            if (!window.CodeMirror) {
                setTimeout(initCodeMirror, 50);
                return;
            }

            loadScript("codemirror/javascript.js");
            loadScript("codemirror/util/simple-hint.js");
            loadScript("codemirror/util/javascript-hint.js");
            loadStyle("codemirror/util/simple-hint.css");

            propertyEditor.editor = CodeMirror.fromTextArea(element("#edit-field"), {
                lineNumbers: true,
                matchBrackets: true,
                tabSize: 4,
                indentUnit: 4,
                indentWithTabs: false
            });
        }

        setTimeout(initCodeMirror, 50);
    }

    var map = {};
    var mapEditor = element(".map-editor");

    function loadMapEditor() {
        loadScript("kinetic.js");

        map.areas = {};
        map.exits = {};

        function openMap() {

            mapEditor.style.display = "block";
            mapEditor.canvas = element(".map-editor .canvas");

            var stage = new Kinetic.Stage({
                container: mapEditor.canvas,
                width: mapEditor.canvas.clientWidth,
                height: mapEditor.canvas.clientHeight
            });

            var layer = new Kinetic.Layer();

            stage.add(layer);

            mapEditor.mainLayer = layer;

            initMap();
        }

        function closeMap() {

            mapEditor.style.display = "none";
        }

        function objectId(string) {
            if (typeof string === "string") {
                if (string.contains(":")) {
                    return parseInt(string.split(":")[1], 10);
                } else {
                    return parseInt(string, 10);
                }
            } else {
                return string;
            }
        }

        function initMap() {

            self.sendApiCall("areas1", "areas-list", function(data) {
                console.log("Received areas");

                for (var i = 0; i < data.length; i++) {
                    var area = data[i];
                    map.areas[objectId(area.id)] = area;
                }

                self.sendApiCall("exits1", "exits-list", function(data) {
                    console.log("Received exits");

                    for (var i = 0; i < data.length; i++) {
                        var exit = data[i];
                        map.exits[objectId(exit.id)] = exit;
                    }

                    layoutAreas();
                    drawMap();
                });
            });
        }

        function layoutAreas() {

            var area;
            for (var key in map.areas) {
                area = map.areas[key];
            }
            if (!area) {
                console.log("No areas to layout");
                return;
            }

            var visited = {};

            var directions = {
                "north":     { "x":  0, "y": -1 },
                "northeast": { "x":  1, "y": -1 },
                "east":      { "x":  1, "y":  0 },
                "southeast": { "x":  1, "y":  1 },
                "south":     { "x":  0, "y":  1 },
                "southwest": { "x": -1, "y":  1 },
                "west":      { "x": -1, "y":  0 },
                "northwest": { "x": -1, "y": -1 }
            };
            var verticals = {
                "up":   { "x":  1, "y": -1 },
                "down": { "x": -1, "y":  1 }
            };

            map.top = map.left = map.bottom = map.right = 0;

            function visitArea(area, x, y) {
                area.x = x;
                area.y = y;

                visited[area.id] = true;

                map.top = (map.top > y ? y : map.top);
                map.left = (map.left > x ? x : map.left);
                map.bottom = (map.bottom < y ? y : map.bottom);
                map.right = (map.right < x ? x : map.right);

                if (!area.exits) {
                    console.log("Area \"" + area.name + "\" has no exits");
                    return;
                }

                var directionalExits = [];
                var verticalExits = [];
                var otherExits = [];
                for (var i = 0; i < area.exits.length; i++) {
                    var exit =  map.exits[objectId(area.exits[i])];
                    console.log(exit);
                    if (directions.hasOwnProperty(exit.name)) {
                        directionalExits.push(exit);
                    } else if (verticals.hasOwnProperty(exit.name)) {
                        verticalExits.push(exit);
                    } else {
                        otherExits.push(exit);
                    }
                }

                var takenSpots = {};
                for (i = 0; i < directionalExits.length; i++) {
                    exit = directionalExits[i];
                    var destinationArea = map.areas[objectId(exit.destinationArea)];
                    if (!destinationArea) {
                        console.log("Exit \"" + exit.name + "\" from area \"" + area.name + "\" has a non-existant destination area");
                        continue;
                    }

                    var spot = directions[exit.name].x + ":" + directions[exit.name].y;
                    takenSpots[spot] = true;

                    if (!visited.hasOwnProperty(destinationArea.id)) {
                        visitArea(destinationArea, x + directions[exit.name].x, y + directions[exit.name].y);
                    }
                }

                for (i = 0; i < verticalExits.length; i++) {
                    exit = verticalExits[i];
                    destinationArea = map.areas[objectId(exit.destinationArea)];
                    if (!destinationArea) {
                        console.log("Exit \"" + exit.name + "\" from area \"" + area.name + "\" has a non-existant destination area");
                        continue;
                    }

                    spot = verticals[exit.name].x + ":" + verticals[exit.name].y;
                    if (takenSpots.hasOwnProperty(spot)) {
                        var freeSpot;
                        for (var key in directions) {
                            spot = directions[key].x + ":" + directions[key].y;
                            if (!takenSpots.hasOwnProperty(spot)) {
                                freeSpot = spot;
                                break;
                            }
                        }
                        if (!freeSpot) {
                            console.log("There are no more free spots for exit \"" + exit.name + "\" in area \"" + area.name + "\"");
                            break;
                        }
                        spot = freeSpot;
                    }
                    takenSpots[spot] = true;

                    if (!visited.hasOwnProperty(destinationArea.id)) {
                        visitArea(destinationArea, x + parseInt(spot.split(":")[0], 10), y + parseInt(spot.split(":")[1], 10));
                    }
                }

                for (i = 0; i < otherExits.length; i++) {
                    exit = otherExits[i];
                    destinationArea = map.areas[objectId(exit.destinationArea)];
                    if (!destinationArea) {
                        console.log("Exit \"" + exit.name + "\" from area \"" + area.name + "\" has a non-existant destination area");
                        continue;
                    }

                    for (var key in directions) {
                        spot = directions[key].x + ":" + directions[key].y;
                        if (!takenSpots.hasOwnProperty(spot)) {
                            freeSpot = spot;
                            break;
                        }
                    }
                    if (!freeSpot) {
                        console.log("There are no more free spots for exit \"" + exit.name + "\" in area \"" + area.name + "\"");
                        break;
                    }
                    takenSpots[freeSpot] = true;

                    if (!visited.hasOwnProperty(destinationArea.id)) {
                        visitArea(destinationArea, x + parseInt(freeSpot.split(":")[0], 10), y + parseInt(freeSpot.split(":")[1], 10));
                    }
                }
            }

            visitArea(area, 0, 0);
        }

        function drawMap() {

            var area;
            for (var key in map.areas) {
                area = map.areas[key];
            }
            if (!area) {
                console.log("No areas to draw");
                return;
            }

            var visited = {};

            function visitArea(area) {
                visited[area.id] = true;

                if (!area.exits) {
                    return;
                }

                for (var i = 0; i < area.exits.length; i++) {
                    var exit = area.exits[i];
                    var destinationArea = map.areas[objectId(exit.destinationArea)];
                    if (visited.hasOwnProperty(destinationArea.id)) {
                        var line = new Kinetic.Line({
                            points: [mapEditor.canvas.clientWidth / 2 + 40 * area.x, mapEditor.canvas.clientHeight / 2 + 40 * area.y,
                                     mapEditor.canvas.clientWidth / 2 + 40 * destinationArea.x, mapEditor.canvas.clientHeight / 2 + 40 * destinationArea.y],
                            stroke: "red",
                            strokeWidth: 2,
                            lineCap: "round",
                            lineJoin: "round"
                        });
                        mapEditor.mainLayer.add(line);
                    } else {
                        visitArea(destinationArea);
                    }
                }
            }

            visitArea(area);
        }

        var editMapLink = document.createElement("a");
        editMapLink.setAttribute("href", "javascript:void(0)");
        editMapLink.textContent = "Edit Map";
        editMapLink.addEventListener("click", openMap, false);

        var statusHeader = element(".status-header");
        statusHeader.appendChild(document.createTextNode(" "));
        statusHeader.appendChild(editMapLink);
    }

    loadPropertyEditor();
    loadMapEditor();

    var triggers = {};
    self.sendApiCall("triggers1", "triggers-list", function(data) {
        for (var i = 0; i < data.length; i++) {
            var trigger = data[i];
            var triggerName;
            if (trigger.contains("(")) {
                triggerName = trigger.substr(0, trigger.indexOf("("));
            } else {
                triggerName = trigger.substr(0, trigger.indexOf(" : "));
            }
            triggers[triggerName] = trigger;
        }
    });

    var onkeypress = controller.commandInput.onkeypress;
    function substituteCommand(event, commandToExecute, commandToPushToHistory) {
        self.commandInput.value = commandToExecute;
        if (commandToExecute.isEmpty()) {
            self.history.push(commandToPushToHistory);
        } else {
            onkeypress(event);
            self.history[self.history.length - 1] = commandToPushToHistory;
        }
    }

    var saveCommand;

    controller.commandInput.onkeypress = function(event) {
        if (event.keyCode === keys.KEY_RETURN) {
            var command = self.commandInput.value;
            var commandName = command.split(" ")[0];
            var rest = command.substr(commandName.length);

            if (commandName === "edit-prop" || commandName === "edit-p") {
                substituteCommand(event, "", command);

                self.sendApiCall("prop1", "property-get" + rest, function(data) {
                    var propertyName = data.propertyName;
                    if (data.readOnly) {
                        self.writeToScreen("Property " + propertyName + " is read-only.");
                        return;
                    }

                    propertyEditor.show(data[propertyName]);

                    saveCommand = "set-prop #" + data.id + " " + propertyName;
                });
            } else if (commandName === "edit-trigger" || commandName === "edit-t") {
                substituteCommand(event, "", command);

                var triggerName = command.split(/\s+/)[2];
                if (parseInt(triggerName, 10) > 0) {
                    triggerName = command.split(/\s+/)[3];
                }

                if (triggers.hasOwnProperty(triggerName)) {
                    self.sendApiCall("trigger1", "trigger-get" + rest, function(data) {
                        var triggerSource = data.triggerSource;
                        if (triggerSource.isEmpty()) {
                            var trigger = triggers[triggerName];
                            if (trigger.contains("(")) {
                                var arguments = trigger.substring(trigger.indexOf("(") + 1,
                                                                  trigger.indexOf(")"));
                                var args = [];
                                arguments.split(", ").forEach(function(arg) {
                                    args.push(arg.substr(0, arg.indexOf(" : ")));
                                });
                                triggerSource = "(function(" + args.join(", ") + ") {\n    \n})";
                            } else {
                                triggerSource = "(function() {\n    \n})";
                            }
                        }

                        propertyEditor.show(triggerSource);

                        saveCommand = "api-trigger-set #" + data.id + " " + triggerName;
                    });
                } else {
                    self.writeToScreen("There is no trigger named " + triggerName + ".");
                }
            } else if (command === "exec-script") {
                substituteCommand(event, "", command);

                propertyEditor.show();

                saveCommand = "exec-script";
            } else if (command.startsWith("@")) {
                if (command.contains(" ")) {
                    substituteCommand(event, "set-prop area " + command.substr(1), command);
                } else {
                    substituteCommand(event, "get-prop area " + command.substr(1), command);
                }
            } else {
                onkeypress(event);
            }
        } else {
            onkeypress(event);
        }
    };

    self.commandInput.removeAttribute("maxlength");
})();
