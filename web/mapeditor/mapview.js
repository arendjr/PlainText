import { hideLoader, showLoader } from "../loadingwidget/loading.js";

const ROOM_SIZE = 30;
const ROOM_BORDER_WIDTH = 2;

export const SVG_URI = "http://www.w3.org/2000/svg";

export default class MapView {
    constructor(container) {
        this.container = container;

        this.model = null;

        this.zRestriction = null;

        this.selectedRoomId = 0;

        this.selectionListeners = new Set();

        this.center = { x: 0, y: 0 };
        this.zoom = 1.0;
        this.perspective = 0.0;
        this.displayRoomNames = false;

        this.roomFills = {};

        this.attachListeners();
    }

    attachListeners() {
        this.container.addEventListener("click", event => {
            const el = event.target;
            this.selectedRoomId = Number.parseInt(el.dataset.roomId ?? 0);

            this.notifySelectionListeners();
        });
    }

    setModel(model) {
        this.model = model;

        this.model.bind("change", () => {
            if (this.selectedRoomId !== 0) {
                if (!this.model.rooms.hasOwnProperty(this.selectedRoomId)) {
                    this.selectedRoomId = 0;
                    this.notifySelectionListeners();
                }
            }

            this.draw();

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

    draw(options = {}) {
        const canvasWidth = options.width ?? this.container.clientWidth;
        const canvasHeight = options.height ?? this.container.clientHeight;

        this.container.setAttribute(
            "viewBox",
            `0 0 ${canvasWidth} ${canvasHeight}`
        );

        const center = options.center ?? this.center;
        const offsetX = options.offsetX ?? Math.floor(canvasWidth / 2);
        const offsetY = options.offsetY ?? Math.floor(canvasHeight / 2);

        const zoom = 10 * this.zoom;

        const minX = -ROOM_SIZE;
        const maxX = canvasWidth + ROOM_SIZE;
        const minY = -ROOM_SIZE;
        const maxY = canvasHeight + ROOM_SIZE;

        const isWithinCanvas =
            options.isWithinCanvas ??
            ((x, y) => x >= minX && x <= maxX && y >= minY && y <= maxY);

        const zRestriction = this.zRestriction;

        const portals = Object.values(this.model.portals).filter(portal => {
            if (!portal.room || !portal.room2) {
                return false;
            }

            const isVisible =
                zoom >= 2 &&
                (zRestriction === null ||
                    portal.room.z === zRestriction ||
                    portal.room2.z === zRestriction);
            return isVisible;
        });

        // FIXME: We should retain what we can between draw calls:
        this.container.innerHTML = "";

        for (const portal of portals) {
            const { room, room2 } = portal;
            const z1 = this.perspective * room.z;
            const x1 = offsetX + (room.x - center.x + z1) * zoom;
            const y1 = offsetY + (room.y - center.y - z1) * zoom;
            const z2 = this.perspective * room2.z;
            const x2 = offsetX + (room2.x - center.x + z2) * zoom;
            const y2 = offsetY + (room2.y - center.y - z2) * zoom;

            if (isWithinCanvas(x1, y1) || isWithinCanvas(x2, y2)) {
                const flags = portal.flags.split("|");
                const isPassable =
                    flags.includes("CanPassThrough") ||
                    flags.includes("CanPassThroughIfOpen");

                const line = document.createElementNS(SVG_URI, "line");
                line.setAttribute("x1", x1);
                line.setAttribute("y1", y1);
                line.setAttribute("x2", x2);
                line.setAttribute("y2", y2);
                line.setAttribute("stroke", isPassable ? "blue" : "green");
                line.setAttribute("stroke-width", ROOM_BORDER_WIDTH);
                line.setAttribute("opacity", isPassable ? 1.0 : 0.4);
                this.container.appendChild(line);
            }
        }

        const rooms = Object.values(this.model.rooms).filter(room => {
            const isVisible = zRestriction === null || room.z === zRestriction;
            return isVisible;
        });

        for (const room of rooms) {
            const z = this.perspective * room.z;
            const x = offsetX + (room.x - center.x + z) * zoom;
            const y = offsetY + (room.y - center.y - z) * zoom;

            if (isWithinCanvas(x, y)) {
                const rect = document.createElementNS(SVG_URI, "rect");
                rect.setAttribute("data-room-id", room.id);
                rect.setAttribute("x", x - ROOM_SIZE / 2);
                rect.setAttribute("y", y - ROOM_SIZE / 2);
                rect.setAttribute("width", ROOM_SIZE);
                rect.setAttribute("height", ROOM_SIZE);
                rect.setAttribute("fill", this.roomFills[room.id] ?? "grey");
                rect.setAttribute("stroke", "black");
                rect.setAttribute("stroke-width", ROOM_BORDER_WIDTH);
                rect.style.cursor = "pointer";
                this.container.appendChild(rect);

                if (this.displayRoomNames) {
                    const text = document.createElementNS(SVG_URI, "text");
                    text.textContent = room.name ?? "";
                    text.setAttribute("x", x);
                    text.setAttribute("y", y - ROOM_SIZE);
                    text.style.fontSize = 14;
                    this.container.appendChild(text);
                }
            }
        }
    }

    exportSvg() {
        showLoader();

        return Promise.resolve().then(() => {
            const dimensions = this.getMapDimensions();
            this.draw({
                width:
                    dimensions.width +
                    (this.displayRoomNames ? 4 : 2) * ROOM_SIZE,
                height:
                    dimensions.height +
                    (this.displayRoomNames ? 4 : 2) * ROOM_SIZE,
                offsetX: ROOM_SIZE + (self.displayRoomNames ? ROOM_SIZE : 0),
                offsetY: ROOM_SIZE + (self.displayRoomNames ? ROOM_SIZE : 0),
                center: dimensions.topLeft,
                isWithinStage: () => true,
            });

            const svg = this.container.outerHTML;

            hideLoader();
            this.draw();

            return svg;
        });
    }

    print() {
        const target = window.open("", "_blank");
        this.exportSvg().then(data => {
            target.document.write(data);
            target.window.print();
        });
    }

    plotData(data) {
        let lowest = 0;
        let highest = 0;
        for (const id in data) {
            const value = data[id];
            if (value > highest) {
                highest = value;
            } else if (value < lowest) {
                lowest = value;
            }
        }

        const { rooms } = this.model;
        for (const id in rooms) {
            const value = data[id] ?? 0;

            let red = 0;
            let green = 0;
            let blue = 0;
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

            const fill = `rgb(${red}, ${green}, ${blue})`;
            this.roomFills[id] = fill;

            const { shape } = rooms[id];
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
        this.zRestriction =
            zRestriction == null ? null : Number.parseInt(zRestriction);

        this.draw();
    }

    setDisplayRoomNames(displayRoomNames) {
        if (displayRoomNames !== this.displayRoomNames) {
            this.displayRoomNames = !!displayRoomNames;

            this.draw();
        }
    }

    getMapDimensions() {
        let minX = 0;
        let minY = 0;
        let maxX = 0;
        let maxY = 0;
        for (const room of Object.values(this.model.rooms)) {
            const z = this.perspective * room.z;
            const x = room.x + z;
            const y = room.y + z;
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
