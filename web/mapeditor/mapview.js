import { hideLoader, showLoader } from "../loadingwidget/loading.js";

var ROOM_SIZE = 30;
var ROOM_BORDER_WIDTH = 2;

export default class MapView {
    constructor(container) {
        this.container = container;

        this.model = null;

        this.canvas = null;

        this.zRestriction = null;

        this.selectedRoomId = 0;

        this.selectionListeners = new Set();

        this.center = { x: 0, y: 0 };
        this.zoom = 1.0;
        this.perspective = 0.0;
        this.displayRoomNames = false;

        this.roomFills = {};

        this.canvas = new fabric.Canvas(this.container.children().first()[0]);
        this.canvas.selection = false;
        this.canvas.renderOnAddition = false;
        this.canvas.hoverCursor = "pointer";

        this.attachListeners();
    }

    attachListeners() {
        var self = this;

        this.canvas.on("object:selected", function (event) {
            var shape = event.target;
            if (shape.get("id")) {
                self.selectedRoomId = shape.get("id");
            } else {
                self.selectedRoomId = 0;
            }

            self.notifySelectionListeners();
        });
    }

    setModel(model) {
        this.model = model;

        var self = this;
        this.model.bind("change", function () {
            if (self.selectedRoomId !== 0) {
                if (!self.model.rooms.hasOwnProperty(self.selectedRoomId)) {
                    self.selectedRoomId = 0;
                    self.notifySelectionListeners();
                }
            }

            self.draw();

            hideLoader();
        });
    }

    move(dx, dy) {
        this.center.x += (5.1 - 5 * this.zoom) * 20 * dx;
        this.center.y += (5.1 - 5 * this.zoom) * 20 * dy;

        this.draw();
    }

    setZoom(zoom) {
        zoom = 0.1 + 0.9 * zoom;
        if (zoom !== this.zoom) {
            this.zoom = zoom;

            this.draw();
        }
    }

    setPerspective(perspective) {
        perspective = 2 * perspective;
        if (perspective !== this.perspective) {
            this.perspective = perspective;

            this.draw();
        }
    }

    draw(options) {
        options = options || {};

        var canvasWidth = options.width || this.container.width();
        var canvasHeight = options.height || this.container.height();

        this.canvas.setWidth(canvasWidth);
        this.canvas.setHeight(canvasHeight);

        var center = options.center || this.center;
        var offsetX =
            options.offsetX === undefined
                ? Math.floor(canvasWidth / 2)
                : options.offsetX;
        var offsetY =
            options.offsetY === undefined
                ? Math.floor(canvasHeight / 2)
                : options.offsetY;

        var zoom = 10 * this.zoom;

        var minX = -ROOM_SIZE,
            maxX = canvasWidth + ROOM_SIZE,
            minY = -ROOM_SIZE,
            maxY = canvasHeight + ROOM_SIZE;
        var isWithinCanvas =
            options.isWithinCanvas ||
            function (x, y) {
                return x >= minX && x <= maxX && y >= minY && y <= maxY;
            };

        var zRestriction = this.zRestriction;

        const { portals } = this.model;
        const portalIds = Object.values(portals)
            .filter(portal => {
                if (!portal.room || !portal.room2) {
                    return false;
                }

                const isVisible =
                    zoom >= 2 &&
                    (zRestriction === null ||
                        portal.room.z === zRestriction ||
                        portal.room2.z === zRestriction);
                return isVisible;
            })
            .map(portal => portal.id);

        const { rooms } = this.model;
        const roomIds = Object.values(rooms)
            .filter(room => {
                const isVisible =
                    zRestriction === null || room.z === zRestriction;
                return isVisible;
            })
            .map(room => room.id);

        this.canvas.clear();

        var self = this;
        portalIds.forEach(function (id) {
            var portal = portals[id];
            var room = portal.room;
            var room2 = portal.room2;
            var z1 = self.perspective * room.z;
            var x1 = offsetX + (room.x - center.x + z1) * zoom;
            var y1 = offsetY + (room.y - center.y - z1) * zoom;
            var z2 = self.perspective * room2.z;
            var x2 = offsetX + (room2.x - center.x + z2) * zoom;
            var y2 = offsetY + (room2.y - center.y - z2) * zoom;

            if (isWithinCanvas(x1, y1) || isWithinCanvas(x2, y2)) {
                var flags = portal.flags.split("|");
                var isPassable =
                    flags.includes("CanPassThrough") ||
                    flags.includes("CanPassThroughIfOpen");
                var color = isPassable ? "blue" : "green";
                var opacity = isPassable ? 1.0 : 0.4;
                var shape = new fabric.Line([x1, y1, x2, y2], {
                    stroke: color,
                    strokeWidth: ROOM_BORDER_WIDTH,
                    selectable: false,
                    opacity: opacity,
                });
                self.canvas.add(shape);
            }
        });

        roomIds.forEach(function (id) {
            var room = rooms[id];
            var z = self.perspective * room.z;
            var x = offsetX + (room.x - center.x + z) * zoom;
            var y = offsetY + (room.y - center.y - z) * zoom;

            if (isWithinCanvas(x, y)) {
                var fill = self.roomFills[room.id] ?? "grey";
                var shape = new fabric.Rect({
                    id: room.id,
                    left: x,
                    top: y,
                    fill: fill,
                    width: ROOM_SIZE,
                    height: ROOM_SIZE,
                    stroke: "black",
                    strokeWidth: ROOM_BORDER_WIDTH,
                    selectable: true,
                    lockMovementX: true,
                    lockMovementY: true,
                    lockScalingX: true,
                    lockScalingY: true,
                    lockRotation: true,
                    hasControls: false,
                });
                self.canvas.add(shape);

                if (self.displayRoomNames) {
                    var textShape = new fabric.Text(room.name || "", {
                        left: x,
                        top: y - ROOM_SIZE,
                        fontSize: 14,
                        selectable: false,
                    });
                    self.canvas.add(textShape);
                }
            }
        });

        this.canvas.renderAll();
    }

    exportSvg(options) {
        options = options || {};

        showLoader();

        var target = window.open("", "_blank");

        var self = this;
        setTimeout(function () {
            var dimensions = self.getMapDimensions();
            self.draw({
                width:
                    dimensions.width +
                    (self.displayRoomNames ? 4 : 2) * ROOM_SIZE,
                height:
                    dimensions.height +
                    (self.displayRoomNames ? 4 : 2) * ROOM_SIZE,
                offsetX: ROOM_SIZE + (self.displayRoomNames ? ROOM_SIZE : 0),
                offsetY: ROOM_SIZE + (self.displayRoomNames ? ROOM_SIZE : 0),
                center: dimensions.topLeft,
                isWithinStage: function (x, y) {
                    return true;
                },
            });

            if (options.callback) {
                options.callback(target, self.canvas.toSVG());
            } else {
                target.document.location =
                    "data:image/svg+xml;utf8," + self.canvas.toSVG();
            }

            self.draw();

            hideLoader();
        }, 1);
    }

    print() {
        this.exportSvg({
            callback: function (target, data) {
                target.document.write(data);
                target.window.print();
            },
        });
    }

    plotData(data) {
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
            value = data[id] ?? 0;

            var red = 0,
                green = 0,
                blue = 0;
            if (lowest === 0) {
                if (highest === 0) {
                    blue = 255;
                } else {
                    red = Math.floor((255 * value) / highest);
                    blue = 255 - red;
                }
            } else {
                if (value > 0) {
                    red = Math.floor((255 * value) / highest);
                    green = 255 - red;
                } else if (value === 0) {
                    green = 255;
                } else {
                    blue = Math.floor((255 * value) / lowest);
                    green = 255 - blue;
                }
            }

            var fill = "rgb(" + red + ", " + green + ", " + blue + ")";
            this.roomFills[id] = fill;

            var shape = rooms[id].shape;
            if (shape) {
                shape.set("fill", fill);
            }
        }

        this.draw();
    }

    addSelectionListener(listener) {
        this.selectionListeners.add(listener);
    }

    getSelectionListeners() {
        return new Set(this.selectionListeners.values());
    }

    removeSelectionListener(listener) {
        this.selectionListeners.delete(listener);
    }

    removeSelectionListeners() {
        this.selectionListeners.clear();
    }

    setSelectionListeners(listeners) {
        this.selectionListeners = listeners;
    }

    notifySelectionListeners() {
        for (const listener of this.selectionListeners) {
            listener(this.selectedRoomId);
        }
    }

    setZRestriction(zRestriction) {
        if (zRestriction === undefined || zRestriction === null) {
            this.zRestriction = null;
        } else {
            this.zRestriction = parseInt(zRestriction, 10);
        }

        this.draw();
    }

    setDisplayRoomNames(displayRoomNames) {
        if (displayRoomNames !== this.displayRoomNames) {
            this.displayRoomNames = !!displayRoomNames;

            this.draw();
        }
    }

    getMapDimensions() {
        var minX = 0,
            minY = 0,
            maxX = 0,
            maxY = 0;
        for (var id in this.model.rooms) {
            var room = this.model.rooms[id];
            var z = this.perspective * room.z;
            var x = room.x + z;
            var y = room.y + z;
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
            width: 10 * this.zoom * (maxX - minX),
            height: 10 * this.zoom * (maxY - minY),
            topLeft: {
                x: minX,
                y: minY,
            },
        };
    }
}
