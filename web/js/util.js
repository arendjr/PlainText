/*global define:false, require:false*/
define(function() {

    "use strict";

    var directions = [
        { "name": "north", "opposite": "south", "vector": [0, -1, 0] },
        { "name": "northeast", "opposite": "southwest", "vector": [1, -1, 0] },
        { "name": "east", "opposite": "west", "vector": [1, 0, 0] },
        { "name": "southeast", "opposite": "northwest", "vector": [1, 1, 0] },
        { "name": "south", "opposite": "north", "vector": [0, 1, 0] },
        { "name": "southwest", "opposite": "northeast", "vector": [-1, -1, 0] },
        { "name": "west", "opposite": "east", "vector": [-1, 0, 0] },
        { "name": "northwest", "opposite": "southeast", "vector": [-1, -1, 0] },
        { "name": "up", "opposite": "down", "vector": [0, 0, 1] },
        { "name": "down", "opposite": "up", "vector": [0, 0, -1] }
    ];

    function indexOfDirection(direction) {

        for (var i = 0; i < 10; i++) {
            if (directions[i].name === direction) {
                return i;
            }
        }

        return -1;
    }

    function isDirection(string) {

        return indexOfDirection(string) > -1;
    }

    function opposingDirection(direction) {

        return directions[indexOfDirection(direction)].opposite;
    }

    function directionVector(direction) {

        return directions[indexOfDirection(direction)].vector;
    }

    return {
        "isDirection": isDirection,
        "opposingDirection": opposingDirection,
        "directionVector": directionVector
    };
});
