/*global define:false, require:false*/
define(["kinetic"], function(Kinetic) {

    "use strict";

    function MapView(element) {

        this.element = element;

        this.model = null;

        this.stage = null;
        this.layer = null;
        this.shapes = [];

        this.visibleAreas = [];

        this.selectedShape = null;
        this.selectedRoomId = 0;

        this.selectionListeners = [];

        this.center = { "x": 0, "y": 0 };
        this.zoom = 1.0;
        this.perspective = 0.0;

        this.init();
    }

    MapView.prototype.init = function() {

        this.stage = new Kinetic.Stage({
            "container": this.element,
            "width": this.element.clientWidth,
            "height": this.element.clientHeight
        });
        this.layer = new Kinetic.Layer();
        this.stage.add(this.layer);

        this.attachListeners();
    };

    MapView.prototype.attachListeners = function() {

        var self = this;

        this.layer.on("click", function(event) {
            if (self.selectedShape) {
                self.selectedShape.setStroke("black");
            }

            if (event.shape.getId()) {
                self.selectedShape = event.shape;
                self.selectedShape.setStroke("orange");

                self.selectedRoomId = event.shape.getId();
            } else {
                self.selectedShape = null;
                self.selectedRoomId = 0;
            }

            self.layer.draw();

            self.notifySelectionListeners();
        });
    };

    MapView.prototype.setModel = function(model) {

        this.model = model;

        var self = this;
        var firstUpdate = true;

        this.model.addChangeListener(function() {
            if (self.selectedRoomId !== 0) {
                if (!self.model.rooms.contains(self.selectedRoomId)) {
                    self.selectedRoomId = 0;
                    self.notifySelectionListeners();
                }
            }

            if (firstUpdate) {
                self.visibleAreas = [];
                for (var id in self.model.areas) {
                    self.visibleAreas.append(self.model.areas[id]);
                }
                firstUpdate = false;
            }

            self.draw();
        });
    };

    MapView.prototype.move = function(dx, dy) {

        this.center.x += (5.1 - 5 * this.zoom) * 20 * dx;
        this.center.y += (5.1 - 5 * this.zoom) * 20 * dy;

        this.draw();
    };

    MapView.prototype.setZoom = function(zoom) {

        zoom = (0.1 + 0.9 * zoom);
        if (zoom !== this.zoom) {
            this.zoom = zoom;

            this.draw();
        }
    };

    MapView.prototype.setPerspective = function(perspective) {

        if (perspective !== this.perspective) {
            this.perspective = perspective;

            this.draw();
        }
    };

    MapView.prototype.draw = function() {

        var stageWidth = this.element.clientWidth;
        var stageHeight = this.element.clientHeight;

        this.stage.setWidth(stageWidth);
        this.stage.setHeight(stageHeight);

        var centerX = Math.floor(stageWidth / 2);
        var centerY = Math.floor(stageHeight / 2);
        var zoom = this.zoom * 10;

        var roomSize = 30;
        var halfRoomSize = roomSize / 2;

        var minX = -roomSize, maxX = stageWidth + roomSize,
            minY = -roomSize, maxY = stageHeight + roomSize;
        function isWithinStage(x, y) {
            return (x >= minX && x <= maxX && y >= minY && y <= maxY);
        }

        var portals = this.model.portals;
        var portalIds = [];
        for (var portalId in portals) {
            var portal = portals[portalId];
            if (zoom >= 4 &&
                (!portal.room.area || this.visibleAreas.contains(portal.room.area)) &&
                (!portal.room2.area || this.visibleAreas.contains(portal.room2.area))) {
                portalIds.append(portalId);
            } else {
                delete portal.shape;
            }
        }

        var rooms = this.model.rooms;
        var roomIds = [];
        for (var roomId in rooms) {
            var room = rooms[roomId];
            if (!room.area || this.visibleAreas.contains(room.area)) {
                roomIds.append(roomId);
            } else {
                delete room.shape;
            }
        }

        var self = this;
        var perspective = 2 * this.perspective;

        var processedShapes = [];
        var shapesToProcess = this.shapes;

        portalIds.forEach(function(id) {
            var portal = portals[id];
            var room = portal.room;
            var room2 = portal.room2;
            var z = perspective * room.z;
            var z2 = perspective * room2.z;
            var points = [centerX + (room.x - self.center.x + z) * zoom,
                          centerY + (room.y - self.center.y + z) * zoom,
                          centerX + (room2.x - self.center.x + z2) * zoom,
                          centerY + (room2.y - self.center.y + z2) * zoom];

            if (isWithinStage(points[0], points[1]) || isWithinStage(points[2], points[3])) {
                if (portal.shape) {
                    portal.shape.setPoints(points);
                } else {
                    portal.shape = new Kinetic.Line({
                        "points": points,
                        "stroke": "blue",
                        "strokeWidth": 2,
                        "listening": false
                    });
                    self.layer.add(portal.shape);
                }

                shapesToProcess.removeOne(portal.shape);
                processedShapes.append(portal.shape);
            } else {
                delete portal.shape;
            }
        });

        roomIds.forEach(function(id) {
            var room = rooms[id];
            var z = perspective * room.z;
            var x = centerX + (room.x - self.center.x + z) * zoom - halfRoomSize;
            var y = centerY + (room.y - self.center.y + z) * zoom - halfRoomSize;

            if (isWithinStage(x, y)) {
                if (room.shape) {
                    room.shape.setX(x);
                    room.shape.setY(y);
                    room.shape.moveToTop();
                } else {
                    room.shape = new Kinetic.Rect({
                        "id": room.id,
                        "x": x,
                        "y": y,
                        "width": roomSize,
                        "height": roomSize,
                        "stroke": (room.id === self.selectedRoomId ? "orange" : "black"),
                        "strokeWidth": 2,
                        "fill": "grey"
                    });
                    self.layer.add(room.shape);
                }

                shapesToProcess.removeOne(room.shape);
                processedShapes.append(room.shape);
            } else {
                delete room.shape;
            }
        });

        this.shapes = processedShapes;
        shapesToProcess.forEach(function(shape) {
            self.layer.remove(shape);
        });

        this.stage.draw();
    };

    MapView.prototype.plotData = function(data) {

        var lowest = 0;
        var highest = 0;
        var value;
        for (var id in data) {
            value = data[id];
            if (value > highest) {
                highest = value;
            } else if (value < lowest) {
                lowest = value;
            }
        }

        var rooms = this.model.rooms;

        for (id in rooms) {
            var shape = rooms[id].shape;
            if (!shape) {
                continue;
            }

            value = data.contains(id) ? data[id] : 0;

            var red = 0, green = 0, blue = 0;
            if (lowest === 0) {
                if (highest === 0) {
                    blue = 255;
                } else {
                    red = Math.floor(255 * value / highest);
                    blue = 255 - red;
                }
            } else {
                if (value > 0) {
                    red = Math.floor(255 * value / highest);
                    green = 255 - red;
                } else if (value === 0) {
                    green = 255;
                } else {
                    blue = Math.floor(255 * value / lowest);
                    green = 255 - blue;
                }
            }

            shape.setFill("rgb(" + red + ", " + green + ", " + blue + ")");
        }

        this.stage.draw();
    };

    MapView.prototype.addSelectionListener = function(listener) {

        this.selectionListeners.insert(listener);
    };

    MapView.prototype.getSelectionListeners = function() {

        return this.selectionListeners.slice(0);
    };

    MapView.prototype.removeSelectionListener = function(listener) {

        this.selectionListeners.removeAll(listener);
    };

    MapView.prototype.removeSelectionListeners = function() {

        this.selectionListeners.clear();
    };

    MapView.prototype.setSelectionListeners = function(listeners) {

        this.selectionListeners = listeners;
    };

    MapView.prototype.notifySelectionListeners = function() {

        var self = this;

        this.selectionListeners.forEach(function(listener) {
            listener(self.selectedRoomId);
        });
    };

    MapView.prototype.isAreaVisible = function(area) {

        return this.visibleAreas.contains(area);
    };

    MapView.prototype.setAreaVisible = function(area, visible) {

        if (visible) {
            this.visibleAreas.insert(area);
        } else {
            this.visibleAreas.removeOne(area);
        }

        this.draw();
    };

    return MapView;
});
