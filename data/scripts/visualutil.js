
var VisualUtil = (function() {

    var UNDER_QUART_PI = Math.PI / 4.01;
    var OVER_QUART_PI = Math.PI / 3.99;

    var groupsTemplate = {
        "left": [],
        "right": [],
        "ahead": [],
        "behind": [],
        "center": [],
        "above": [],
        "left wall": [],
        "right wall": [],
        "wall": [],
        "ceiling": []
    };

    function descriptionForGroup(group) {
        switch (group) {
            case "left":        return [ "To your left", "is", "are" ];
            case "right":       return [ "To your right", "is", "are" ];
            case "ahead":       return [ "Ahead of you, there", "is", "are" ];
            case "behind":      return [ "Behind you", "is", "are" ];
            case "above":       return [ "Above you", "is", "are" ];
            case "left wall":   return [ "On the left wall", "hangs", "hang" ];
            case "right wall":  return [ "On the right wall", "hangs", "hang" ];
            case "wall":        return [ "On the wall", "hangs", "hang" ];
            case "ceiling":     return [ "From the ceiling", "hangs", "hang" ];
            case "distance":    return [ "In the distance", "you see", "you see" ];
            case "roof":        return [ "On the roof", "you see", "you see" ];
            default:            return [ "There", "is", "are" ];
        }
    }

    function divideItemsIntoGroups(items, direction) {

        var groups = groupsTemplate.clone();
        for (var i = 0, length = items.length; i < length; i++) {
            var item = items[i];
            if (item.hidden) {
                continue;
            }

            var flags = item.flags.split("|");
            var angle = Util.angleBetweenXYVectors(direction, item.position);

            if (flags.contains("AttachedToCeiling")) {
                groups["ceiling"].push(item);
            } else if (flags.contains("AttachedToWall")) {
                if ((item.position[0] === 0 && item.position[1] === 0) ||
                    Math.abs(angle) > 3 * OVER_QUART_PI || Math.abs(angle) < UNDER_QUART_PI) {
                    groups["wall"].push(item);
                } else if (angle > 0) {
                    groups["right wall"].push(item);
                } else {
                    groups["left wall"].push(item);
                }
            } else if (item.position[0] === 0 && item.position[1] === 0) {
                groups["center"].push(item);
            } else {
                if (Math.abs(angle) > 3 * OVER_QUART_PI) {
                    groups["behind"].push(item);
                } else if (Math.abs(angle) < UNDER_QUART_PI) {
                    groups["ahead"].push(item);
                } else if (angle > 0) {
                    groups["right"].push(item);
                } else {
                    groups["left"].push(item);
                }
            }
        }
        return groups;
    }

    function dividePortalsAndCharactersIntoGroups(character, room, strength) {

        var groups = groupsTemplate.clone();
        for (var i = 0, length = room.portals.length; i < length; i++) {
            var portal = room.portals[i];
            if (portal.isHiddenFromRoom(room)) {
                continue;
            }

            var position = portal.position.minus(room.position);
            var angle = Util.angleBetweenXYVectors(character.direction, position);

            if (portal.canSeeThrough() && Math.abs(angle) < UNDER_QUART_PI) {
                groups["characters"] = charactersVisibleThroughPortal(character, room,
                                                                      portal, strength);
            }

            var name = portal.nameFromRoom(room);
            if (Util.isDirection(name) || name === "out") {
                continue;
            }

            if (Math.abs(angle) > 3 * OVER_QUART_PI) {
                groups["behind"].push(portal);
            } else if (Math.abs(angle) < UNDER_QUART_PI) {
                groups["ahead"].push(portal);
            } else if (angle > 0) {
                groups["right"].push(portal);
            } else {
                groups["left"].push(portal);
            }
        }
        return groups;
    }

    function charactersVisibleThroughPortal(character, sourceRoom, portal,
                                            strength, excludedRooms) {

        var room = portal.oppositeOf(sourceRoom);
        var roomStrength = (strength || sourceRoom.eventMultiplier("Visual")) *
                           portal.eventMultiplier("Visual") *
                           room.eventMultiplier("Visual");
        if (roomStrength < 0.1) {
            return [];
        }

        var vector1 = room.position.minus(sourceRoom.position);
        var distance = vector1.vectorLength();

        var visitedRooms = excludedRooms || {};
        visitedRooms[sourceRoom.id] = true;
        visitedRooms[room.id] = true;

        var characters = [];
        room.characters.forEach(function(character) {
            characters.push({
                "character": character,
                "strength": roomStrength,
                "distance": distance
            });
        });

        room.portals.forEach(function(nextPortal) {
            if (!nextPortal.canSeeThrough()) {
                return;
            }

            var nextRoom = nextPortal.oppositeOf(room);
            if (nextRoom === sourceRoom || visitedRooms.hasOwnProperty(nextRoom.id)) {
                return;
            }

            var vector2 = nextRoom.position.minus(room.position);

            var flags = room.flags.split("|");
            if (flags.contains("HasWalls")) {
                if (vector1[0] !== vector2[0] || vector1[1] !== vector2[1]) {
                    return;
                }
            } else {
                var vector3 = nextRoom.position.minus(character.currentRoom.position);
                var angle = Util.angleBetweenXYVectors(character.direction, vector3);
                if (Math.abs(angle) > UNDER_QUART_PI) {
                    return;
                }
            }

            if ((flags.contains("HasCeiling") && vector2[2] > vector1[2]) ||
                (flags.contains("HasFloor") && vector2[2] < vector1[2])) {
                return;
            }

            characters.append(charactersVisibleThroughPortal(character, room, nextPortal,
                                                             roomStrength, visitedRooms));
        });

        return characters;
    }

    function describeCharactersRelativeTo(characters, relative) {

        if (!characters || characters.length === 0) {
            return "";
        }

        var groups = {
            "distance": [],
            "ahead": [],
            "roof": []
        };

        var sentences = [];

        var characterInfo;
        for (var i = 0, length = characters.length; i < length; i++) {
            characterInfo = characters[i];
            var flags = characterInfo.character.currentRoom.flags.split("|");
            if (flags.contains("IsRoof")) {
                groups["roof"].push(characterInfo);
            } else if (characterInfo.distance > 50) {
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
                characterInfo = group[i];
                var character = characterInfo.character;
                if (character && character.group && character.group.leader === character) {
                    group.removeAt(i);
                    character.group.members.forEach(function(member) {
                        for (var i = 0; i < group.length; i++) {
                            if (group[i].character === member) {
                                group.removeAt(i);
                                return;
                            }
                        }
                    });
                    group.push({
                        "group": character.group,
                        "strength": characterInfo.strength,
                        "distance": characterInfo.distance
                    });
                    i = 0;
                }
            }

            var prefix = descriptionForGroup(key)[0];

            var infos = [], numMen = 0, numWomen = 0, numUnknown = 0, numPeople = 0;
            for (i = 0; i < group.length; i++) {
                characterInfo = group[i];

                var name, actionDescription;
                if (characterInfo.group) {
                    name = characterInfo.group.nameAtStrength(characterInfo.strength);
                    if (numPeople > 0 || i < group.length - 1) {
                        name = "a group " + (name.startsWith("a lot ") ? "with " : "of ") +
                                            (name.startsWith("some ") ? name.mid(5) : name);
                    }

                    actionDescription = describeActionRelativeTo(characterInfo.group.leader,
                                                                 relative, characterInfo.distance);

                    infos.push({
                        "group": characterInfo.group,
                        "name": name,
                        "action": actionDescription.text
                    });
                    numPeople += 1 + characterInfo.group.members.length;
                } else {
                    if (characterInfo.character.race.name === "animal") {
                        continue;
                    }

                    name = characterInfo.character.nameAtStrength(characterInfo.strength);

                    actionDescription = describeActionRelativeTo(characterInfo.character,
                                                                 relative, characterInfo.distance);

                    if (name === "a man") {
                        numMen++;
                    } else if (name === "a woman") {
                        numWomen++;
                    } else if (name === "someone") {
                        numUnknown++;
                    } else {
                        infos.push({
                            "character": characterInfo.character,
                            "name": name,
                            "action": actionDescription.text
                        });
                    }
                    numPeople++;
                }

                if (actionDescription.target) {
                    for (var j = i + 1; j < group.length; j++) {
                        if (group[j].character === actionDescription.target) {
                            group.removeAt(j);
                            break;
                        }
                    }
                }
            }

            for (i = 0; i < infos.length; i++) {
                if (!infos[i].character) {
                    continue;
                }

                var count = 1;
                for (j = i + 1; j < infos.length; j++) {
                    if (infos[i].name === infos[j].name && infos[i].action === infos[j].action) {
                        infos.removeAt(j);
                        count++;
                        j--;
                    }
                }
                if (count > 1) {
                    infos[i].name = writtenAmount(count) + " " + infos[i].character.plural;
                }
            }

            var characterTexts = [], hasMan = false, hasWoman = false;
            for (i = 0, length = infos.length; i < length; i++) {
                var info = infos[i];
                name = info.name;
                if (info.action !== "") {
                    name += " " + info.action;
                }
                characterTexts.push(name);

                if (info.character) {
                    if (info.character.gender === "male") {
                        hasMan = true;
                    } else if (info.character.gender === "female") {
                        hasWoman = true;
                    }
                } else if (info.group) {
                    [info.group.leader].concat(info.group.members).forEach(function(member) {
                        if (member.gender === "male") {
                            hasMan = true;
                        } else if (member.gender === "female") {
                            hasWoman = true;
                        }
                    });
                }
            }

            if (numUnknown === 0) {
                if (numMen === 0) {
                    if (numWomen === 1) {
                        if (hasWoman) {
                            characterTexts.push("another woman");
                        } else {
                            characterTexts.push("a woman");
                        }
                    } else if (numWomen > 1) {
                        if (hasWoman) {
                            characterTexts.push(writtenAmount(numWomen) + " other women");
                        } else {
                            characterTexts.push(writtenAmount(numWomen) + " women");
                        }
                    }
                } else if (numMen === 1) {
                    if (numWomen <= 1) {
                        if (hasMan) {
                            characterTexts.push("another man");
                        } else {
                            characterTexts.push("a man");
                        }
                        if (numWomen === 1) {
                            if (hasWoman && !hasMan) {
                                characterTexts.push("another woman");
                            } else {
                                characterTexts.push("a woman");
                            }
                        }
                    } else {
                        if (hasWoman) {
                            characterTexts.push(writtenAmount(numWomen) + " other women");
                        } else {
                            characterTexts.push(writtenAmount(numWomen) + " women");
                        }
                        if (hasMan && !hasWoman) {
                            characterTexts.push("another man");
                        } else {
                            characterTexts.push("a man");
                        }
                    }
                } else {
                    if (numWomen <= 1) {
                        if (hasMan) {
                            characterTexts.push(writtenAmount(numMen) + " other men");
                        } else {
                            characterTexts.push(writtenAmount(numMen) + " men");
                        }
                        if (numWomen === 1) {
                            if (hasWoman && !hasMan) {
                                characterTexts.push("another woman");
                            } else {
                                characterTexts.push("a woman");
                            }
                        }
                    } else {
                        if (numPeople > numMen + numWomen) {
                            characterTexts.push(writtenAmount(numMen + numWomen) +
                                                  " other people");
                        } else {
                            characterTexts.push(writtenAmount(numMen + numWomen) + " people");
                        }
                    }
                }
            } else {
                numUnknown += numMen + numWomen;
                if (numPeople > numUnknown) {
                    if (numUnknown === 1) {
                        characterTexts.push("someone else");
                    } else {
                        characterTexts.push(writtenAmount(numUnknown) + " other people");
                    }
                } else {
                    if (numUnknown === 1) {
                        characterTexts.push("someone");
                    } else {
                        characterTexts.push(writtenAmount(numUnknown) + " people");
                    }
                }
            }

            if (!characterTexts.isEmpty()) {
                sentences.push("%1, you see %2.".arg(prefix, Util.joinFancy(characterTexts)));
            }
        }

        return sentences.join(" ");
    }

    function describeActionRelativeTo(character, relative, distance) {

        var text = "", target, action = character.currentAction;
        if (action === "walk" || action === "run") {
            var continuous = (action === "walk" ? "walking" : "running");

            var vector = character.currentRoom.position.minus(relative.currentRoom.position);
            var angle = character.direction.angle(vector);

            var direction;
            if (Math.abs(angle) < UNDER_QUART_PI) {
                direction = "away from you";
            } else if (Math.abs(angle) > 3 * OVER_QUART_PI && distance <= 100) {
                direction = (distance > 100 ? "in your direction" : "toward you");
            } else {
                direction = Util.directionForVector(character.direction);
            }

            text = "%1 %2".arg(continuous, direction);
        } else if (action === "fight") {
            if (character.target && character.target.currentRoom === character.currentRoom) {
                target = character.target;
                text = "fighting " + target.indefiniteName();
            } else {
                text = "fighting";
            }
        } else if (action === "guard") {
            if (character.target) {
                target = character.target;
                if (target.isPortal()) {
                    text = "guarding the " + target.nameFromRoom(character.currentRoom);
                } else {
                    text = "guarding " + target.indefiniteName();
                }
            } else {
                text = "standing guard";
            }
        }

        return { "text": text, "target": target };
    }

    function writtenAmount(amount) {

        if (amount === 1) {
            return "one";
        } else if (amount === 2) {
            return "two";
        } else if (amount < 6) {
            return "some";
        } else {
            return "a lot of";
        }
    }

    return {
        "descriptionForGroup": descriptionForGroup,
        "divideItemsIntoGroups": divideItemsIntoGroups,
        "dividePortalsAndCharactersIntoGroups": dividePortalsAndCharactersIntoGroups,
        "charactersVisibleThroughPortal": charactersVisibleThroughPortal,
        "describeCharactersRelativeTo": describeCharactersRelativeTo,
        "describeActionRelativeTo": describeActionRelativeTo
    };
})();
