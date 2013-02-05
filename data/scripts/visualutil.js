
var VisualUtil = (function() {

    function charactersVisibleThroughPortal(sourceRoom, portal, strength) {

        var room = portal.oppositeOf(sourceRoom);
        var roomStrength = (strength || sourceRoom.eventMultiplier("Visual")) *
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

        var characterInfo;
        for (var i = 0, length = characters.length; i < length; i++) {
            characterInfo = characters[i];
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
                    group.append({
                        "group": character.group,
                        "strength": characterInfo.strength,
                        "distance": characterInfo.distance
                    });
                    i = 0;
                }
            }

            var prefix = (key === "distance" ?
                          Util.randomAlternative("In the distance", "From afar") :
                          "Ahead of you");

            var infos = [], numMen = 0, numWomen = 0, numUnknown = 0, numPeople = 0;
            for (i = 0; i < group.length; i++) {
                characterInfo = group[i];

                var name, actionDescription;
                if (characterInfo.group) {
                    name = characterInfo.group.nameAtStrength(characterInfo.strength);
                    if (numPeople > 0 || i < group.length - 1) {
                        name = "a group of " + (name.startsWith("some ") ? name.mid(5) : name);
                    }

                    actionDescription = describeActionRelativeTo(characterInfo.group.leader,
                                                                 relative, characterInfo.distance);

                    infos.append({
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
                        infos.append({
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
                            return;
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
                characterTexts.append(name);

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
                            characterTexts.append("another woman");
                        } else {
                            characterTexts.append("a woman");
                        }
                    } else if (numWomen > 1) {
                        if (hasWoman) {
                            characterTexts.append(writtenAmount(numWomen) + " other women");
                        } else {
                            characterTexts.append(writtenAmount(numWomen) + " women");
                        }
                    }
                } else if (numMen === 1) {
                    if (numWomen <= 1) {
                        if (hasMan) {
                            characterTexts.append("another man");
                        } else {
                            characterTexts.append("a man");
                        }
                        if (numWomen === 1) {
                            if (hasWoman && !hasMan) {
                                characterTexts.append("another woman");
                            } else {
                                characterTexts.append("a woman");
                            }
                        }
                    } else {
                        if (hasWoman) {
                            characterTexts.append(writtenAmount(numWomen) + " other women");
                        } else {
                            characterTexts.append(writtenAmount(numWomen) + " women");
                        }
                        if (hasMan && !hasWoman) {
                            characterTexts.append("another man");
                        } else {
                            characterTexts.append("a man");
                        }
                    }
                } else {
                    if (numWomen <= 1) {
                        if (hasMan) {
                            characterTexts.append(writtenAmount(numMen) + " other men");
                        } else {
                            characterTexts.append(writtenAmount(numMen) + " men");
                        }
                        if (numWomen === 1) {
                            if (hasWoman && !hasMan) {
                                characterTexts.append("another woman");
                            } else {
                                characterTexts.append("a woman");
                            }
                        }
                    } else {
                        if (numPeople > numMen + numWomen) {
                            characterTexts.append(writtenAmount(numMen + numWomen) +
                                                  " other people");
                        } else {
                            characterTexts.append(writtenAmount(numMen + numWomen) + " people");
                        }
                    }
                }
            } else {
                numUnknown += numMen + numWomen;
                if (numPeople > numUnknown) {
                    if (numUnknown === 1) {
                        characterTexts.append("someone else");
                    } else {
                        characterTexts.append(writtenAmount(numUnknown) + " other people");
                    }
                } else {
                    if (numUnknown === 1) {
                        characterTexts.append("someone");
                    } else {
                        characterTexts.append(writtenAmount(numUnknown) + " people");
                    }
                }
            }

            if (!characterTexts.isEmpty()) {
                sentences.append("%1, you see %2.".arg(prefix, Util.joinFancy(characterTexts)));
            }
        }

        return sentences.join(" ");
    }

    function describeActionRelativeTo(character, relative, distance) {

        var text = "", target, action = character.currentAction;
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
        "charactersVisibleThroughPortal": charactersVisibleThroughPortal,
        "describeVisibleCharactersRelativeTo": describeVisibleCharactersRelativeTo,
        "describeActionRelativeTo": describeActionRelativeTo
    };
})();
