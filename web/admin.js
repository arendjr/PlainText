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

            element(".selected-area").style.display = "none";

            mapEditor.style.display = "block";
            mapEditor.canvas = element(".map-editor .canvas");

            mapEditor.stage = new Kinetic.Stage({
                container: mapEditor.canvas,
                width: mapEditor.canvas.clientWidth,
                height: mapEditor.canvas.clientHeight,
                draggable: true
            });

            initMap();
        }

        function closeMap() {

            mapEditor.style.display = "none";
        }

        function selectArea(areaId) {

            var area = map.areas[areaId];
            element(".selected-area .name").value = area.name;
            element(".selected-area .description").value = area.description;
            element(".selected-area").style.display = "block";
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
                        var destinationArea = map.areas[objectId(exit.destinationArea)];
                        if (destinationArea) {
                            exit.destinationArea = destinationArea;
                            map.exits[objectId(exit.id)] = exit;
                        } else {
                            console.log("Exit " + exit.id + " has a non-existant destination area");
                        }
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
            var orderedDirections = [ "north", "northeast", "east", "southeast",
                                      "south", "southwest", "west", "northwest" ];
            var verticals = {
                "up":   { "x":  1, "y": -1 },
                "down": { "x": -1, "y":  1 }
            };

            map.top = map.left = map.bottom = map.right = 0;

            function visitArea(area, x, y) {
                area.x = x;
                area.y = y;

                visited[area.id] = true;

                map.top = (y < map.top ? y : map.top);
                map.left = (x < map.left ? x : map.left);
                map.bottom = (y > map.bottom ? y : map.bottom);
                map.right = (x > map.right ? x : map.right);

                if (!area.exits) {
                    console.log("Area \"" + area.name + "\" has no exits");
                    return;
                }

                var directionalExits = [];
                var verticalExits = [];
                var otherExits = [];
                for (var i = 0; i < area.exits.length; i++) {
                    var exit = map.exits[objectId(area.exits[i])];
                    if (directions.hasOwnProperty(exit.name)) {
                        directionalExits.push(exit);
                    } else if (verticals.hasOwnProperty(exit.name)) {
                        verticalExits.push(exit);
                    } else {
                        otherExits.push(exit);
                    }
                }

                var takenSpots = {};
                function findSpot(preferredX, preferredY) {
                    var spot = preferredX + ":" + preferredY;
                    if (takenSpots.hasOwnProperty(spot)) {
                        var start = (preferredY === 1 ? 4 : 0);
                        var length = orderedDirections.length;
                        for (var i = start; i < start + length; i++) {
                            var direction = directions[orderedDirections[i % length]];
                            spot = direction.x + ":" + direction.y;
                            if (!takenSpots.hasOwnProperty(spot)) {
                                takenSpots[spot] = true;
                                return { "x": direction.x, "y": direction.y };
                            }
                        }
                        console.log("There are no more free spots in area \"" + area.name + "\"");
                        return {};
                    } else {
                        takenSpots[spot] = true;
                        return { "x": preferredX, "y": preferredY };
                    }
                }

                directionalExits.forEach(function(exit) {
                    var spot = findSpot(directions[exit.name].x, directions[exit.name].y);
                    if (!visited.hasOwnProperty(exit.destinationArea.id)) {
                        visitArea(exit.destinationArea, x + spot.x, y + spot.y);
                    }
                });

                verticalExits.forEach(function(exit) {
                    var spot = findSpot(verticals[exit.name].x, verticals[exit.name].y);
                    if (!visited.hasOwnProperty(exit.destinationArea.id)) {
                        visitArea(exit.destinationArea, x + spot.x, y + spot.y);
                    }
                });

                otherExits.forEach(function(exit) {
                    var spot = findSpot(-1, 0);
                    if (!visited.hasOwnProperty(exit.destinationArea.id)) {
                        visitArea(exit.destinationArea, x + spot.x, y + spot.y);
                    }
                });
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

            var gridSize = 60;

            var layer = new Kinetic.Layer();
            var centerX = mapEditor.canvas.clientWidth / 2 - gridSize * (map.right + map.left);
            var centerY = mapEditor.canvas.clientWidth / 2 - gridSize * (map.bottom + map.top);

            function visitArea(area) {
                visited[area.id] = true;

                if (!area.exits) {
                    return;
                }

                for (var i = 0; i < area.exits.length; i++) {
                    var exit = map.exits[objectId(area.exits[i])];
                    if (visited.hasOwnProperty(exit.destinationArea.id)) {
                        var line = new Kinetic.Line({
                            points: [centerX + gridSize * area.x,
                                     centerY + gridSize * area.y,
                                     centerX + gridSize * exit.destinationArea.x,
                                     centerY + gridSize * exit.destinationArea.y],
                            stroke: "blue",
                            strokeWidth: 2
                        });
                        layer.add(line);
                    } else {
                        visitArea(exit.destinationArea);
                    }
                }

                var rect = new Kinetic.Rect({
                    id: objectId(area.id),
                    x: centerX + gridSize * area.x - 15,
                    y: centerY + gridSize * area.y - 15,
                    width: gridSize / 2,
                    height: gridSize / 2,
                    stroke: "black",
                    strokeWidth: 2,
                    fill: "grey"
                });
                layer.add(rect);
            }

            visitArea(area);

            layer.on("click", function(event) {
                if (event.shape.getId()) {
                    selectArea(event.shape.getId());
                }
            });
            mapEditor.stage.add(layer);
        }

        var editMapLink = document.createElement("a");
        editMapLink.setAttribute("href", "javascript:void(0)");
        editMapLink.textContent = "Edit Map";
        editMapLink.addEventListener("click", openMap, false);

        var statusHeader = element(".status-header");
        statusHeader.appendChild(document.createTextNode(" "));
        statusHeader.appendChild(editMapLink);

        element(".map-editor .close").onclick = closeMap;
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
