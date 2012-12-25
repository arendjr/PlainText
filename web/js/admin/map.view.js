/*global define:false, require:false*/
define(["fabric", "zepto"], function(Fabric, $) {

    "use strict";

    var ROOM_SIZE = 30;
    var ROOM_BORDER_WIDTH = 2;

    function MapView(container) {

        this.container = container;

        this.model = null;

        this.canvas = null;

        this.visibleAreas = [];
        this.zRestriction = null;

        this.selectedShape = null;
        this.selectedRoomId = 0;

        this.selectionListeners = [];

        this.center = { "x": 0, "y": 0 };
        this.zoom = 1.0;
        this.perspective = 0.0;
        this.displayRoomNames = false;

        this.init();
    }

    MapView.prototype.init = function() {

        this.canvas = new Fabric.Canvas(this.container.children().first()[0]);
        this.canvas.selection = false;
        this.canvas.renderOnAddition = false;

        this.attachListeners();
    };

    MapView.prototype.attachListeners = function() {

        var self = this;

        this.canvas.on("object:selected", function(event) {
            var shape = event.target;
            if (shape.get("id")) {
                self.selectedShape = shape;
                self.selectedRoomId = shape.get("id");
            } else {
                self.selectedShape = null;
                self.selectedRoomId = 0;
            }

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

                $(".loading").hide();

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

        perspective = 2 * perspective;
        if (perspective !== this.perspective) {
            this.perspective = perspective;

            this.draw();
        }
    };

    MapView.prototype.draw = function(options) {

        options = options || {};

        var canvasWidth = options.width || this.container.width();
        var canvasHeight = options.height || this.container.height();

        this.canvas.setWidth(canvasWidth);
        this.canvas.setHeight(canvasHeight);

        var center = options.center || this.center;
        var offsetX = options.offsetX === undefined ? Math.floor(canvasWidth / 2) : options.offsetX;
        var offsetY = options.offsetY === undefined ? Math.floor(canvasHeight / 2): options.offsetY;

        var zoom = 10 * this.zoom;

        var minX = -ROOM_SIZE, maxX = canvasWidth + ROOM_SIZE,
            minY = -ROOM_SIZE, maxY = canvasHeight + ROOM_SIZE;
        var isWithinCanvas = options.isWithinCanvas || function(x, y) {
            return x >= minX && x <= maxX && y >= minY && y <= maxY;
        }

        var visibleAreas = this.visibleAreas;
        var zRestriction = this.zRestriction;

        var portals = this.model.portals;
        var portalIds = [];
        for (var portalId in portals) {
            var portal = portals[portalId];
            var isVisible = (zoom >= 3 &&
                             (!portal.room.area || visibleAreas.contains(portal.room.area)) &&
                             (!portal.room2.area || visibleAreas.contains(portal.room2.area)) &&
                             (zRestriction === null || portal.room.z === zRestriction ||
                                                       portal.room2.z === zRestriction));
            if (isVisible) {
                portalIds.append(portalId);
            } else {
                delete portal.shape;
            }
        }

        var rooms = this.model.rooms;
        var roomIds = [];
        for (var roomId in rooms) {
            var room = rooms[roomId];
            isVisible = ((!room.area || visibleAreas.contains(room.area)) &&
                         (zRestriction === null || room.z === zRestriction));
            if (isVisible) {
                roomIds.append(roomId);
            } else {
                delete room.shape;
            }
        }

        var self = this;

        var shapesToProcess = this.canvas.getObjects().clone();

        portalIds.forEach(function(id) {
            var portal = portals[id];
            var room = portal.room;
            var room2 = portal.room2;
            var z1 = self.perspective * room.z;
            var x1 = offsetX + (room.x - center.x + z1) * zoom;
            var y1 = offsetY + (room.y - center.y + z1) * zoom;
            var z2 = self.perspective * room2.z;
            var x2 = offsetX + (room2.x - center.x + z2) * zoom;
            var y2 = offsetY + (room2.y - center.y + z2) * zoom;

            if (isWithinCanvas(x1, y1) || isWithinCanvas(x2, y2)) {
                if (portal.shape) {
                    portal.shape.set({
                        "x1": x1, "y1": y1,
                        "x2": x2, "y2": y2
                    });
                } else {
                    portal.shape = new Fabric.Line([ x1, y1, x2, y2 ], {
                        "fill": "blue",
                        "stroke": "blue",
                        "strokeWidth": ROOM_BORDER_WIDTH,
                        "selectable": false
                    });
                    self.canvas.add(portal.shape);
                }

                shapesToProcess.removeOne(portal.shape);
            } else {
                delete portal.shape;
            }
        });

        roomIds.forEach(function(id) {
            var room = rooms[id];
            var z = self.perspective * room.z;
            var x = offsetX + (room.x - center.x + z) * zoom;
            var y = offsetY + (room.y - center.y + z) * zoom;

            if (isWithinCanvas(x, y)) {
                if (room.shape) {
                    room.shape.set({
                        "left": x,
                        "top": y
                    });
                    room.shape.bringToFront();
                } else {
                    room.shape = new Fabric.Rect({
                        "id": room.id,
                        "left": x,
                        "top": y,
                        "fill": "grey",
                        "width": ROOM_SIZE,
                        "height": ROOM_SIZE,
                        "stroke": "black",
                        "strokeWidth": ROOM_BORDER_WIDTH,
                        "selectable": true,
                        "lockMovementX": true,
                        "lockMovementY": true,
                        "lockScalingX": true,
                        "lockScalingY": true,
                        "lockRotation": true,
                        "hasControls": false
                    });
                    self.canvas.add(room.shape);

                    if (room.id === self.selectedRoomId) {
                        self.selectedShape = room.shape;
                    }
                }
                shapesToProcess.removeOne(room.shape);

                if (self.displayRoomNames) {
                    if (room.textShape) {
                        room.textShape.set({
                            "left": x,
                            "top": y - ROOM_SIZE
                        });
                        room.textShape.bringToFront();
                    } else {
                        room.textShape = new Fabric.Text(room.name || "", {
                            "left": x,
                            "top": y - ROOM_SIZE,
                            "fontSize": 14,
                            "selectable": false
                        });
                        self.canvas.add(room.textShape);
                    }
                    shapesToProcess.removeOne(room.textShape);
                } else {
                    delete room.textShape;
                }
            } else {
                delete room.shape;
                delete room.textShape;
            }
        });

        shapesToProcess.forEach(function(shape) {
            self.canvas.remove(shape);
        });

        this.canvas.renderAll();
    };

    MapView.prototype.export = function(options) {

        options = options || {};

        $(".loading").show();

        var target = window.open("", "_blank");

        var self = this;
        setTimeout(function() {
            var dimensions = self.getMapDimensions();
            self.draw({
                "width": dimensions.width + (self.displayRoomNames ? 4 : 2) * ROOM_SIZE,
                "height": dimensions.height + (self.displayRoomNames ? 4 : 2) * ROOM_SIZE,
                "offsetX": ROOM_SIZE + (self.displayRoomNames ? ROOM_SIZE : 0),
                "offsetY": ROOM_SIZE + (self.displayRoomNames ? ROOM_SIZE : 0),
                "center": dimensions.topLeft,
                "isWithinStage": function(x, y) { return true; }
            });

            if (options.callback) {
                options.callback(target, self.canvas.toSVG());
            } else {
                target.document.location = "data:image/svg+xml;utf8," + self.canvas.toSVG();
            }

            $(".loading").hide();
        }, 1);
    };

    MapView.prototype.print = function() {

        this.export({
            "callback": function(target, data) {
                target.document.write(data);
                target.window.print();
            }
        });
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

            shape.set("fill", "rgb(" + red + ", " + green + ", " + blue + ")");
        }

        this.canvas.renderAll();
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

    MapView.prototype.setZRestriction = function(zRestriction) {

        if (zRestriction === undefined || zRestriction === null) {
            this.zRestriction = null;
        } else {
            this.zRestriction = parseInt(zRestriction, 10);
        }

        this.draw();
    };

    MapView.prototype.setDisplayRoomNames = function(displayRoomNames) {

        if (displayRoomNames !== this.displayRoomNames) {
            this.displayRoomNames = !!displayRoomNames;

            this.draw();
        }
    };

    MapView.prototype.getMapDimensions = function() {

        var minX = 0, minY = 0, maxX = 0, maxY = 0;
        for (var id in this.model.rooms) {
            var room = this.model.rooms[id];
            var z = this.perspective * room.z;
            var x = (room.x + z);
            var y = (room.y + z);
            if (x < minX) {
                minX = x;
            } else if (x > maxX) {
                maxX = x;
            }
            if (y < minY) {
                minY = y;
            } else if (y > maxY) {
                maxY = y;
            }
        }
        return {
            "width": 10 * this.zoom * (maxX - minX),
            "height": 10 * this.zoom * (maxY - minY),
            "topLeft": {
                "x": minX,
                "y": minY
            }
        };
    };

    return MapView;
});
