
var VisualUtil = (function() {

    function charactersVisibleThroughPortal(sourceRoom, portal, strength) {

        var room = portal.oppositeOf(sourceRoom);
        var roomStrength = strength || sourceRoom.eventMultiplier("Visual") *
                           portal.eventMultiplier("Visual") *
                           room.eventMultiplier("Visual");
        if (roomStrength < 0.1) {
            return [];
        }

        var characters = [];
        for (var i = 0, length = room.characters.length; i < length; i++) {
            characters.push({
                "character": room.characters[i],
                "strength": roomStrength,
                "distance": sourceRoom.position.minus(room.position).vectorLength()
            });
        }

        for (i = 0, length = room.portals.length; i < length; i++) {
            var nextPortal = room.portals[i];
            if (!nextPortal.canSeeThrough()) {
                continue;
            }

            var nextRoom = nextPortal.oppositeOf(room);
            if (nextRoom === sourceRoom) {
                continue;
            }

            var vector1 = room.position.minus(sourceRoom.position);
            var vector2 = nextRoom.position.minus(room.position);
            var flags = room.flags.split("|");
            if (flags.contains("HasWalls")) {
                if (vector1[0] !== vector2[0] || vector1[1] !== vector2[1]) {
                    continue;
                }
            }
            if (flags.contains("HasCeiling") && vector2[2] > vector1[2]) {
                continue;
            }
            if (flags.contains("HasFloor") && vector2[2] < vector1[2]) {
                continue;
            }

            characters = characters.concat(charactersVisibleThroughPortal(room, nextPortal,
                                                                          roomStrength));
        }

        return characters;
    }

    function describeVisibleCharactersRelativeTo(characters, relative) {

        if (!characters || characters.length === 0) {
            return "";
        }

        var groups = {
            "distance": [],
            "ahead": []
        };

        var sentences = [];

        for (var i = 0, length = characters.length; i < length; i++) {
            var characterInfo = characters[i];
            if (characterInfo.distance > 50) {
                groups["distance"].push(characterInfo);
            } else {
                groups["ahead"].push(characterInfo);
            }
        }

        for (var key in groups) {
            if (!groups.hasOwnProperty(key) || groups[key].length === 0) {
                continue;
            }

            var group = groups[key];
            for (i = 0; i < group.length; i++) {
                var character = group[i].character;
                if (character && character.group && character.group.leader === character) {
                    group.removeAt(i);
                    character.group.members.forEach(function(member) {
                        for (var i = 0; i < group.length; i++) {
                            if (characters.character === member) {
                                group.removeAt(i);
                                return;
                            }
                        }
                    });
                    group.append({
                        "group": character.group,
                        "strength": group[i].strength,
                        "distance": group[i].distance
                    });
                    i = 0;
                }
            }

            var prefix = (key === "distance" ?
                          Util.randomAlternative("In the distance", "From afar") :
                          "Ahead of you");

            var characterTexts = [], numMen = 0, numWomen = 0, numPeople = 0;
            for (i = 0, length = characters.length; i < length; i++) {
                characterInfo = characters[i];

                var name, actionDescription;
                if (characterInfo.group) {
                    name = characterInfo.group.nameAtStrength(characterInfo.strength);
                    if (numPeople > 0 || i < length - 1) {
                        name = "a group of " + name;
                    }

                    actionDescription = describeActionRelativeTo(characterInfo.group.leader,
                                                                 relative, characterInfo.distance);
                    if (actionDescription !== "") {
                        name += " " + actionDescription;
                    }

                    characterTexts.append(name);
                    numPeople += 1 + characterInfo.group.members.length;
                } else {
                    name = characterInfo.character.nameAtStrength(characterInfo.strength);

                    actionDescription = describeActionRelativeTo(characterInfo.character,
                                                                 relative, characterInfo.distance);
                    if (actionDescription !== "") {
                        name += " " + actionDescription;
                    }

                    if (name === "a man") {
                        numMen++;
                    } else if (name === "a woman") {
                        numWomen++;
                    } else {
                        characterTexts.append(name);
                    }
                    numPeople++;
                }
            }
            if (numWomen === 1) {
                characterTexts.prepend("a woman");
            } else if (numWomen > 1) {
                characterTexts.prepend(Util.writtenNumber(numWomen) + " women");
            }
            if (numMen === 1) {
                characterTexts.prepend("a man");
            } else if (numMen > 1) {
                characterTexts.prepend(Util.writtenNumber(numMen) + " men");
            }
            sentences.append("%1, you see %2.".arg(prefix, Util.joinFancy(characterTexts)));
        }

        return sentences.join(" ");
    }

    function describeActionRelativeTo(character, relative, distance) {

        var action = character.currentAction;
        if (action === "walk" || action === "run") {
            var continuous = (action === "walk" ? "walking" : "running");

            var vector = character.position.minus(relative.position);
            var angle = character.direction.angle(vector);

            var direction;
            if (angle < Math.PI / 4) {
                direction = "away from you";
            } else if (angle >= Math.PI * 3 / 4 && distance <= 100) {
                direction = (distance > 100 ? "in your direction" : "toward you");
            } else {
                direction = Util.directionForVector(character.direction);
            }

            return "%1 %2".arg(continuous, direction);
        } else if (action === "fight") {
            return "fighting";
        } else {
            return "";
        }
    }

    return {
        "charactersVisibleThroughPortal": charactersVisibleThroughPortal,
        "describeVisibleCharactersRelativeTo": describeVisibleCharactersRelativeTo,
        "describeActionRelativeTo": describeActionRelativeTo
    };
})();
