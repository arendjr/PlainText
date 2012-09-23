(function() {

    function Spot(x, y) {
        this.x = x;
        this.y = y;
    }

    Spot.prototype.take = function(area) {
        area.spots[4 + this.x + 3 * this.y] = 1;
    };

    Spot.prototype.isAvailable = function(area) {
        return area.spots[4 + this.x + 3 * this.y] === 0;
    };

    var self = controller;

    var mapEditor = element(".map-editor");

    loadScript("kinetic.js");

    var map = {
        areas: {},
        exits: {}
    };

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

        loadMap();
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

    function loadMap() {
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
            break;
        }
        if (!area) {
            console.log("No areas to layout");
            return;
        }

        var visitedAreas = [];
        var positions = {};

        var directions = {
            "north":     new Spot( 0, -1),
            "northeast": new Spot( 1, -1),
            "east":      new Spot( 1,  0),
            "southeast": new Spot( 1,  1),
            "south":     new Spot( 0,  1),
            "southwest": new Spot(-1,  1),
            "west":      new Spot(-1,  0),
            "northwest": new Spot(-1, -1)
        };
        var orderedDirections = [ "north", "east", "south", "west",
                                  "northeast", "southeast", "southwest", "northwest" ];
        var verticals = {
            "up":   new Spot( 1, -1),
            "down": new Spot(-1,  1)
        };

        map.top = map.left = map.bottom = map.right = 0;

        function visitArea(area, x, y) {
            //console.log("Visting area \"" + area.name + "\"");

            area.x = x;
            area.y = y;
            area.spots = [0, 0, 0,
                          0, 0, 0,
                          0, 0, 0];
            positions[x + ":" + y] = area;

            var visitIndex = visitedAreas.length;
            visitedAreas.push(area);

            function unwindVisitedAreas() {
                for (var i = visitIndex, length = visitedAreas.length; i < length; i++) {
                    var visitedArea = visitedAreas[i];
                    delete positions[visitedArea.x + ":" + visitedArea.y];
                }
                visitedAreas.splice(visitIndex);
            }

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

            function findSpot(preferredSpot) {
                if (preferredSpot.isAvailable(area)) {
                    preferredSpot.take(area);
                    return preferredSpot;
                } else {
                    var start = (preferredSpot.y === 1 ? 2 : 0);
                    var length = orderedDirections.length;
                    for (var i = start; i < start + length; i++) {
                        var spot = directions[orderedDirections[i % length]];
                        if (spot.isAvailable(area)) {
                            spot.take(area);
                            return spot;
                        }
                    }
                    console.log("There are no more free spots in area \"" + area.name + "\"");
                    return null;
                }
            }

            function spotForDestination(destinationArea, originArea) {
                var x = destinationArea.x - originArea.x;
                var y = destinationArea.y - originArea.y;
                if (x !== 0 && y !== 0 && Math.abs(x) !== Math.abs(y)) {
                    return null;
                }
                if (x !== 0) {
                    x /= Math.abs(x);
                }
                if (y !== 0) {
                    y /= Math.abs(y);
                }
                return new Spot(x, y);
            }

            function attemptToVisitArea(area, spot) {
                for (var i = 1; i <= 3; i++) {
                    var destX = x + i * spot.x;
                    var destY = y + i * spot.y;
                    if (positions.hasOwnProperty(destX + ":" + destY)) {
                        return false;
                    }

                    // skim the area
                    for (var dx = -1; dx <= 1; dx++) {
                        for (var dy = -1; dy <= 1; dy++) {
                            if ((dx === 0 && dy === 0) ||
                                (dx === -spot.x && dy === -spot.y)) {
                                continue;
                            }
                            var position = (destX + dx) + ":" + (destY + dy);
                            if (positions.hasOwnProperty(position)) {
                                var adjacentArea = positions[position];
                                if (!(new Spot(-dx, -dy)).isAvailable(adjacentArea)) {
                                    return false;
                                }
                            }
                        }
                    }

                    if (visitArea(area, destX, destY)) {
                        return true;
                    }
                }
                return false;
            }

            var succeeded = true;

            function followExit(exit) {
                if (!succeeded) {
                    return;
                }

                if (visitedAreas.contains(exit.destinationArea)) {
                    var spot = spotForDestination(exit.destinationArea, area);
                    if (spot === null) {
                        succeeded = false;
                        //console.log("  Exit \"" + exit.name + "\" is currently located at " +
                        //              "a non-reachable position.");
                        return;
                    }
                    //console.log("  Exit \"" + exit.name + "\" is currently located at " +
                    //            spot.x + "," + spot.y);
                    if (spot.isAvailable(area)) {
                        spot.take(area);
                    } else {
                        succeeded = false;
                        //console.log("    ... but that spot is already taken");
                    }
                } else {
                    do {
                        if (directions.hasOwnProperty(exit.name)) {
                            if (directions[exit.name].isAvailable(area)) {
                                spot = directions[exit.name];
                                spot.take(area);
                            } else {
                                spot = null;
                            }
                        } else {
                            spot = findSpot(verticals.hasOwnProperty(exit.name) ?
                                            verticals[exit.name] : new Spot(-1, 0));
                        }
                        if (spot === null) {
                            succeeded = false;
                            break;
                        }
                        //console.log("  Exit \"" + exit.name + "\" is going to be at " +
                        //            spot.x + "," + spot.y);
                    } while (!attemptToVisitArea(exit.destinationArea, spot));
                }
            }

            directionalExits.forEach(followExit);
            verticalExits.forEach(followExit);
            otherExits.forEach(followExit);
            if (!succeeded) {
                unwindVisitedAreas();
                return false;
            }

            return true;
        }

        if (!visitArea(area, 0, 0)) {
            console.log("No satisfying solution found");
        }
    }

    function drawMap() {
        var area;
        for (var key in map.areas) {
            area = map.areas[key];
            break;
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

})();
