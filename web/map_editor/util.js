const directions = [
    { name: "north", opposite: "south", vector: [0, -1, 0] },
    { name: "northeast", opposite: "southwest", vector: [1, -1, 0] },
    { name: "east", opposite: "west", vector: [1, 0, 0] },
    { name: "southeast", opposite: "northwest", vector: [1, 1, 0] },
    { name: "south", opposite: "north", vector: [0, 1, 0] },
    { name: "southwest", opposite: "northeast", vector: [-1, 1, 0] },
    { name: "west", opposite: "east", vector: [-1, 0, 0] },
    { name: "northwest", opposite: "southeast", vector: [-1, -1, 0] },
    { name: "up", opposite: "down", vector: [0, 0, 1] },
    { name: "down", opposite: "up", vector: [0, 0, -1] },
];

function indexOfDirection(direction) {
    for (let i = 0; i < 10; i++) {
        if (directions[i].name === direction) {
            return i;
        }
    }

    return -1;
}

export function directionForVector(vector) {
    if (Array.isArray(vector)) {
        vector = { x: vector[0], y: vector[1], z: vector[2] };
    }

    if (
        Math.abs(vector.z) >
        Math.sqrt(Math.pow(vector.x, 2) + Math.pow(vector.y, 2))
    ) {
        return vector.z > 0 ? "down" : "up";
    } else {
        const degrees = (Math.atan2(vector.y, vector.x) * 180.0) / Math.PI;
        if (degrees < -157.5 || degrees > 157.5) {
            return "west";
        } else if (degrees >= -157.5 && degrees < -112.5) {
            return "northwest";
        } else if (degrees >= -112.5 && degrees < -67.5) {
            return "north";
        } else if (degrees >= -67.5 && degrees < -22.5) {
            return "northeast";
        } else if (degrees >= -22.5 && degrees < 22.5) {
            return "east";
        } else if (degrees >= 22.5 && degrees < 67.5) {
            return "southeast";
        } else if (degrees >= 67.5 && degrees < 112.5) {
            return "south";
        } else {
            return "southwest";
        }
    }
}

export function formatRef(parsedRef) {
    return `${parsedRef[0]}:${parsedRef[1]}`;
}

export function getId(ref) {
    return ref ? parseRef(ref)[1] : undefined;
}

export function isDirection(string) {
    return indexOfDirection(string) > -1;
}

export function noop() {}

export function opposingDirection(direction) {
    const directionObject = directions[indexOfDirection(direction)];
    return directionObject ? directionObject.opposite : "";
}

export function parseRef(ref) {
    const [type, id] = ref.split(":", 2);
    return [type, Number.parseInt(id)];
}

export function portalNameFromRoom(portal, room) {
    const [type, id] = parseRef(portal.room);
    return type === "room" && id === room.id ? portal.name : portal.name2;
}

export function vectorForDirection(direction) {
    const directionObject = directions[indexOfDirection(direction)];
    return directionObject ? directionObject.vector : [0, 0, 0];
}
