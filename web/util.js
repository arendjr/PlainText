export const Keys = {
    RETURN: 13,
    LEFT_ARROW: 37,
    UP_ARROW: 38,
    RIGHT_ARROW: 39,
    DOWN_ARROW: 40,
};

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

export function isDirection(string) {
    return indexOfDirection(string) > -1;
}

export function opposingDirection(direction) {
    const directionObject = directions[indexOfDirection(direction)];
    return directionObject ? directionObject.opposite : "";
}

export function vectorForDirection(direction) {
    const directionObject = directions[indexOfDirection(direction)];
    return directionObject ? directionObject.vector : [0, 0, 0];
}

export function directionForVector(vector) {
    if (vector instanceof Array) {
        vector = { x: vector[0], y: vector[1], z: vector[2] };
    }

    if (
        Math.abs(vector.z) >
        Math.sqrt(Math.pow(vector.x, 2) + Math.pow(vector.y, 2))
    ) {
        if (vector.z > 0) {
            return "down";
        } else {
            return "up";
        }
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
