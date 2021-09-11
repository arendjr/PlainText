import { h, shallowEqual, useLayoutEffect, useRef, useState } from "../lib.js";
import { parseRef } from "../util.js";
import { usePortals } from "../stores/portals.js";
import { useRooms } from "../stores/rooms.js";
import { useViewSettings } from "../stores/view_settings.js";

const ROOM_SIZE = 30;
const ROOM_BORDER_WIDTH = 2;
const HALF_ROOM_SIZE = ROOM_SIZE / 2;

export const MapView = function MapView({
    imperativeHandle,
    onSelectRoom,
    roomFills,
    selectedRoomId,
}) {
    const {
        center,
        perspective,
        showRoomNames,
        zRestriction,
        zRestrictionEnabled,
        zoom,
    } = useViewSettings(
        state => ({
            ...state,
            perspective: 2 * state.perspective,
            zoom: 1 + 9 * state.zoom,
        }),
        shallowEqual
    );

    const [width, setWidth] = useState(100);
    const [height, setHeight] = useState(100);
    const svgRef = useRef(null);

    useLayoutEffect(() => {
        setWidth(svgRef.current.clientWidth);
        setHeight(svgRef.current.clientHeight);
    }, []);

    const offsetX = Math.floor(width / 2);
    const offsetY = Math.floor(height / 2);

    const rooms = useRooms(state => state.rooms);
    const portals = usePortals(state => state.portals);

    const getRoom = ref => {
        const [type, id] = parseRef(ref);
        return type === "room" ? rooms.get(id) : undefined;
    };

    const onClick = event => {
        const el = event.target;
        onSelectRoom(Number.parseInt(el.dataset.roomId ?? 0));
    };

    imperativeHandle.exportSvg = () => {
        const viewBox = svgRef.current.getAttribute("viewBox");
        const { x, y, width, height } = getMapDimensions(rooms, {
            perspective,
            showRoomNames,
            zoom,
        });
        svgRef.current.setAttribute(
            "viewBox",
            `${x - center.x} ${y - center.y} ${width} ${height}`
        );
        const svg = svgRef.current.outerHTML;
        svgRef.current.setAttribute("viewBox", viewBox);
        return svg;
    };

    return h(
        "svg",
        {
            class: "map-view",
            onClick,
            ref: svgRef,
            viewBox: `0 0 ${width} ${height}`,
            xmlns: "http://www.w3.org/2000/svg",
        },
        ...Array.from(portals.values())
            .filter(portal => {
                const room = getRoom(portal.room);
                const room2 = getRoom(portal.room2);
                if (!room || !room2) {
                    return false;
                }

                return (
                    !zRestrictionEnabled ||
                    room.position[2] === zRestriction ||
                    room2.position[2] === zRestriction
                );
            })
            .map(portal => {
                const room = getRoom(portal.room);
                const room2 = getRoom(portal.room2);

                const [x1, y1, z1] = room.position;
                const [x2, y2, z2] = room2.position;

                const flags = portal.flags.split("|");
                const isPassable =
                    flags.includes("CanPassThrough") ||
                    flags.includes("CanPassThroughIfOpen");

                return h("line", {
                    key: `p${portal.id}`,
                    x1: offsetX + (x1 - center.x + perspective * z1) * zoom,
                    y1: offsetY + (y1 - center.y - perspective * z1) * zoom,
                    x2: offsetX + (x2 - center.x + perspective * z2) * zoom,
                    y2: offsetY + (y2 - center.y - perspective * z2) * zoom,
                    stroke: isPassable ? "blue" : "green",
                    "stroke-width": ROOM_BORDER_WIDTH,
                    opacity: isPassable ? 1.0 : 0.4,
                });
            }),
        ...Array.from(rooms.values())
            .filter(
                room =>
                    !zRestrictionEnabled || room.position[2] === zRestriction
            )
            .map(room => {
                const [x, y, z] = room.position;

                const warp = perspective * z;
                const middleX = offsetX + (x - center.x + warp) * zoom;
                const middleY = offsetY + (y - center.y - warp) * zoom;

                const rect = h("rect", {
                    key: `r${room.id}`,
                    class: "room",
                    "data-room-id": room.id,
                    x: middleX - HALF_ROOM_SIZE,
                    y: middleY - HALF_ROOM_SIZE,
                    width: ROOM_SIZE,
                    height: ROOM_SIZE,
                    fill: roomFills.get(room.id) ?? "grey",
                    stroke: room.id === selectedRoomId ? "red" : "black",
                    "stroke-width": ROOM_BORDER_WIDTH,
                });

                if (showRoomNames) {
                    const text = h(
                        "text",
                        {
                            key: `rn${room.id}`,
                            class: "room-name",
                            x: middleX,
                            y: middleY - ROOM_SIZE,
                            "dominant-baseline": "middle",
                            "text-anchor": "middle",
                        },
                        room.name
                    );

                    return [rect, text];
                } else {
                    return rect;
                }
            })
    );
};

function getMapDimensions(rooms, { perspective, showRoomNames, zoom }) {
    let minX = 0;
    let minY = 0;
    let maxX = 0;
    let maxY = 0;
    for (const room of rooms.values()) {
        const z = perspective * room.position[2];
        const x = room.position[0] + z;
        const y = room.position[1] + z;
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
        x: zoom * (minX - (showRoomNames ? 2 : 1 * ROOM_SIZE)),
        y: zoom * minY,
        width: zoom * (maxX - minX + (showRoomNames ? 4 : 2) * ROOM_SIZE),
        height: zoom * (maxY - minY),
    };
}
