function MapView(element) {

    this.element = element;

    this.model = null;

    this.stage = null;
    this.layer = null;
    this.shapes = [];

    this.selectedShape = null;
    this.selectedRoomId = 0;

    this.selectionListeners = [];

    this.zoom = 1.0;
    this.perspective = 0.0;

    var self = this;

    loadScript("kinetic.js", function() {
        self.init();
    });
}

MapView.prototype.init = function() {

    this.stage = new Kinetic.Stage({
        "container": this.element,
        "width": this.element.clientWidth,
        "height": this.element.clientHeight,
        "draggable": true
    });
    this.layer = new Kinetic.Layer();
    this.stage.add(this.layer);

    this.attachListeners();

    if (this.model !== null) {
        this.draw();
    }
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

    this.model.addChangeListener(function() {
        if (self.selectedRoomId !== 0) {
            if (!self.model.rooms.contains(self.selectedRoomId)) {
                self.selectedRoomId = 0;
                self.notifySelectionListeners();
            }
        }

        if (self.stage !== null) {
            self.draw();
        }
    });
};

MapView.prototype.setZoom = function(zoom) {

    if (zoom !== this.zoom) {
        this.zoom = zoom;

        this.layer.setScale(zoom);
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

    this.stage.setWidth(this.element.clientWidth);
    this.stage.setHeight(this.element.clientHeight);

    var rooms = this.model.rooms;
    var roomIds = Object.keys(rooms);

    if (roomIds.isEmpty()) {
        console.log("No rooms to draw");
        return;
    }

    var roomSize = 30;
    var halfRoomSize = 15;

    var self = this;
    var perspective = 2 * this.perspective;

    var processedShapes = [];
    var shapesToProcess = this.shapes;

    roomIds.forEach(function(id) {
        var room = rooms[id];
        var z = perspective * room.z;
        for (var i = 0, length = room.exits.length; i < length; i++) {
            var exit = room.exits[i];
            var destination = exit.destination;
            var points = [room.x + z,
                          room.y - z,
                          destination.x + perspective * destination.z,
                          destination.y - perspective * destination.z];
            if (exit.shape) {
                exit.shape.setPoints(points);
            } else {
                exit.shape = new Kinetic.Line({
                    "points": points,
                    "stroke": "blue",
                    "strokeWidth": 2,
                    "listening": false
                });
                self.layer.add(exit.shape);
            }

            shapesToProcess.removeOne(exit.shape);
            processedShapes.append(exit.shape);
        }

        var x = room.x - halfRoomSize + z;
        var y = room.y - halfRoomSize - z;
        if (room.shape) {
            room.shape.setX(x);
            room.shape.setY(y);
        } else {
            room.shape = new Kinetic.Rect({
                "id": room.id,
                "x": x,
                "y": y,
                "width": roomSize,
                "height": roomSize,
                "stroke": "black",
                "strokeWidth": 2,
                "fill": "grey"
            });
            self.layer.add(room.shape);
        }

        shapesToProcess.removeOne(room.shape);
        processedShapes.append(room.shape);
    });
    roomIds.forEach(function(id) {
        rooms[id].shape.moveToTop();
    });

    this.shapes = processedShapes;
    shapesToProcess.forEach(function(shape) {
        self.layer.remove(shape);
    });

    this.stage.draw();
}

MapView.prototype.plotData = function(data) {

    console.log(data);

    var lowest = 0;
    var highest = 0;
    for (var id in data) {
        var value = data[id];
        if (value > highest) {
            highest = value;
        } else if (value < lowest) {
            lowest = value;
        }
    }

    var rooms = this.model.rooms;

    for (id in rooms) {
        var shape = rooms[id].shape;
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
}

MapView.prototype.addSelectionListener = function(listener) {

    this.selectionListeners.insert(listener);
};

MapView.prototype.removeSelectionListener = function(listener) {

    this.selectionListeners.removeAll(listener);
};

MapView.prototype.notifySelectionListeners = function() {

    var self = this;

    this.selectionListeners.forEach(function(listener) {
        listener(self.selectedRoomId);
    });
};

scriptLoaded("map.view.js");
