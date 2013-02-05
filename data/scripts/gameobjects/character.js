
function Character() {

    this.currentAction = "";
    this.currentActionTimerId = 0;
}

Character.prototype.changeStats = function(newStats) {

    this.maxHp = 2 * newStats[VITALITY];
    this.maxMp = newStats[INTELLIGENCE];
};

Character.prototype.close = function(portal) {

    if (!portal.canOpenFromRoom(this.currentRoom)) {
        this.send("Exit cannot be closed.");
        return;
    }

    var name = portal.nameFromRoom(this.currentRoom);
    if (portal.open) {
        if (!portal.invokeTrigger("onclose", this)) {
            return;
        }

        portal.open = false;
        this.send("You close the %1.".arg(name));

        var others = this.currentRoom.characters;
        others.removeOne(this);
        others.send("%1 closes the %2.".arg(this.definiteName(this.currentRoom.characters), name));
    } else {
        this.send("The %1 is already closed.".arg(name));
    }
};

Character.prototype.die = function(attacker) {

    if (!this.invokeTrigger("ondie", attacker)) {
        return;
    }

    this.send("You died.", Color.Red);

    var room = this.currentRoom;
    var myName = this.definiteName(room.characters, Options.Capitalized);

    var others = room.characters;
    others.removeOne(this);
    others.send("%1 died.".arg(myName), Color.Teal);

    if (!this.inventory.isEmpty()) {
        this.inventory.forEach(function(item) {
            room.addItem(item);
        });

        var droppedItemsDescription = this.inventory.joinFancy();
        others.send("%1 was carrying %2.".arg(myName, droppedItemsDescription), Color.Teal);

        this.inventory = [];
    }

    for (var i = 0, length = others.length; i < length; i++) {
        others[i].invokeTrigger("oncharacterdied", this, attacker);
    };

    this.killAllTimers();

    this.setAction("");

    room.removeCharacter(this);

    if (this.isPlayer()) {
        LogUtil.countPlayerDeath(room.toString());

        this.enter(this.race.startingRoom);

        this.hp = 1;
        this.stun(5000);
    } else {
        if (this.respawnTime) {
            var respawnTime = this.respawnTime + randomInt(0, this.respawnTimeVariation);
            this.setTimeout(function() {
                this.hp = this.maxHp;
                this.mp = this.maxMp;

                this.enter(this.currentRoom);

                this.invokeTrigger("onspawn");
            }, respawnTime);
        } else {
            this.setDeleted();
        }
    }
};

Character.prototype.go = function(pointer) {

    if (!pointer) {
        return;
    }

    if (this.secondsStunned() > 0) {
        this.send("Please wait %1 seconds.".arg(this.secondsStunned()), Color.Olive);
        return;
    }

    var i, length, exitName = "", destination = null, portal = null;

    if (pointer.isPortal()) {
        portal = pointer;
        exitName = portal.nameFromRoom(this.currentRoom);
        destination = portal.oppositeOf(this.currentRoom);
    } else if (pointer.isRoom()) {
        for (i = 0, length = this.currentRoom.portals.length; i < length; i++) {
            var potentialPortal = this.currentRoom.portals[i];
            if (potentialPortal.oppositeOf(this.currentRoom) === pointer) {
                portal = potentialPortal;
                exitName = portal.nameFromRoom(this.currentRoom);
                break;
            }
        }
        destination = pointer;
    } else {
        return;
    }

    var character, numCharacters = this.currentRoom.characters.length;
    for (i = 0; i < numCharacters; i++) {
        character = this.currentRoom.characters[i];
        if (character !== this) {
            if (!character.invokeTrigger("oncharacterexit", this, exitName)) {
                return;
            }
        }
    }

    if (portal) {
        if (portal.canOpen() && !portal.open) {
            this.send("The %1 is closed.".arg(exitName));
            return;
        }
        if (!portal.invokeTrigger("onenter", this)) {
            return;
        }
        if (!portal.canPassThrough()) {
            this.send("You cannot go there.");
            return;
        }
    }

    var followers = [];
    if (this.group && this.group.leader === this) {
        for (i = 0, length = this.group.members.length; i < length; i++) {
            var member = this.group.members[i];
            if (member.currentRoom !== this.currentRoom) {
                continue;
            }

            var blocked = false;
            for (var j = 0; j < numCharacters; j++) {
                character = this.currentRoom.characters[j];
                if (!character.invokeTrigger("oncharacterexit", this, exitName)) {
                    blocked = true;
                    break;
                }
            }
            if (blocked) {
                continue;
            }

            if (portal && !portal.invokeTrigger("onenter", member)) {
                continue;
            }

            followers.append(member);
        }
    }

    var source = this.currentRoom;
    var movement = destination.position.minus(source.position);
    var direction = movement.normalized();

    this.leave(source);
    this.direction = direction;
    this.enter(destination);

    var action;
    if (this.currentAction === "walk" || this.currentAction === "run") {
        action = "run";
    } else {
        action = "walk";
    }
    this.setAction(action, { "duration": 4000 });

    for (i = 0, length = followers.length; i < length; i++) {
        var follower = followers[i];
        follower.leave(source);
        follower.direction = direction;
        follower.enter(destination);
        follower.send("You follow %1.".arg(this.name));
        this.setAction(action, { "duration": 4000 });
    }

    var party = [this].concat(followers);

    var simplePresent = (followers.isEmpty() ? action + "s" : action);
    var continuous = (followers.isEmpty() ? "is" : "are") + " " +
                     (action === "walk" ? "walking" : "running");

    var visualEvent = Realm.createEvent("MovementVisual", source, 1.0);
    visualEvent.subject = (followers.isEmpty() ? this : this.group);
    visualEvent.destination = destination
    visualEvent.movement = movement;
    visualEvent.direction = direction;
    visualEvent.setVerb(simplePresent, continuous);
    visualEvent.excludedCharacters = party;
    visualEvent.fire();

    if (this.race && this.race.name !== "animal") {
        var soundStrength = (action === "walk" ? 1.0 : 3.0);
        var soundDescription = "someone";

        if (!followers.isEmpty()) {
            soundDescription = "some people";
            for (i = 0, length = followers.length; i < length; i++) {
                soundStrength += max(0.8 - 0.2 * i, 0.3);
            }
        }

        var soundEvent = Realm.createEvent("MovementSound", source, soundStrength);
        soundEvent.description = soundDescription;
        soundEvent.distantDescription = soundDescription;
        soundEvent.veryDistantDescription = soundDescription;
        soundEvent.destination = destination;
        soundEvent.movement = movement;
        soundEvent.direction = direction;
        soundEvent.setVerb(simplePresent, continuous);
        soundEvent.excludedCharacters = party.concat(visualEvent.affectedCharacters);
        soundEvent.fire();
    }

    if (this.isPlayer()) {
        LogUtil.countRoomVisit(destination.toString(), 1 + followers.length);
    }
};

Character.prototype.guard = function(target) {

    this.setAction("guard", { "target": target });
};

Character.prototype.kill = function(character) {

    if (this.secondsStunned() > 0) {
        this.send("Please wait %1 seconds.".arg(this.secondsStunned()), Color.Olive);
        return;
    }

    if (!character.invokeTrigger("onattack", this)) {
        return;
    }

    var room = this.currentRoom;

    var others = room.characters;
    others.removeOne(this);
    others.removeOne(character);

    var invoked = false;
    if (room.hasTrigger("oncombat")) {
        invoked = room.invokeTrigger("oncombat", this, character, others);
    }
    if (!invoked) {
        Realm.invokeTrigger("oncombat", this, character, others);
    }

    for (var i = 0, length = others.length; i < length; i++) {
        others[i].invokeTrigger("oncharacterattacked", this, character);
    }

    if (character.hp === 0) {
        character.die(this);
    }

    this.setAction("fight", { "target": character, "duration": 4000 });
};

Character.prototype.lookAtBy = function(character) {

    var pool = this.currentRoom.characters;

    var text;
    if (character.id === this.id) {
        text = "You look at yourself.\n";
    } else if (this.isPlayer()) {
        text = "You look at %1, a %2 %3.\n"
               .arg(this.definiteName(pool), this.race.adjective, this.characterClass.name);
    } else {
        text = "You look at %1.\n".arg(this.definiteName(pool));
    }

    if (!this.description.isEmpty()) {
        text += this.description + "\n";
    }

    var wieldedItems = [];
    if (this.weapon) {
        wieldedItems.append(this.weapon);
    }
    if (this.secondaryWeapon) {
        wieldedItems.append(this.secondaryWeapon);
    }
    if (this.shield) {
        wieldedItems.append(this.shield);
    }
    if (!wieldedItems.isEmpty()) {
        text += "%1 is wielding %2.\n".arg(this.subjectPronoun.capitalized())
                                      .arg(wieldedItems.joinFancy());
    }

    if (character.id !== this.id) {
        var observerStatsTotal = character.stats.total();
        if (character.weapon) {
            observerStatsTotal += character.weapon.stats.total();
        }
        var characterStatsTotal = this.stats.total();
        if (this.weapon) {
            characterStatsTotal += this.weapon.stats.total();
        }
        var statsDiff = observerStatsTotal - characterStatsTotal;

        if (statsDiff > 25) {
            if (character.race.name === "giant" && this.race.name !== "giant") {
                text += "You should be careful not to accidentally step on %1.\n"
                        .arg(this.objectPronoun);
            } else if (character.race.name === "goblin" && this.race.name !== "goblin") {
                text += "%1 should be thankful if you don't kill %2.\n"
                        .arg(this.subjectPronoun.capitalized(), this.objectPronoun);
            } else {
                text += "If it came to a fight with %1, %2'd better get %3 friends first.\n"
                        .arg(this.objectPronoun, this.subjectPronoun, this.possessiveAdjective);
            }
        } else if (statsDiff > 15) {
            text += "%1 is not much of a match for you.\n".arg(this.subjectPronoun.capitalized());
        } else if (statsDiff > 5) {
            text += "You appear to be a bit stronger than %1.\n".arg(this.objectPronoun);
        } else if (statsDiff < -25) {
            text += "%1 could kill you with a needle.\n".arg(this.subjectPronoun.capitalized());
        } else if (statsDiff < -15) {
            text += "%1 appears like %2 could put up one hell of a fight.\n"
                    .arg(this.subjectPronoun.capitalized(), this.subjectPronoun);
        } else if (statsDiff < -5) {
            text += "You better think twice before attacking %2.\n".arg(this.objectPronoun);
        } else {
            text += "%1 appears to be about as strong as you are.\n"
                    .arg(this.subjectPronoun.capitalized());
        }
    }

    return text;
};

Character.prototype.maxInventoryWeight = function() {

    return 20 + this.stats[STRENGTH] + Math.floor(this.stats[ENDURANCE] / 2);
};

Character.prototype.nameAtStrength = function(strength) {

    if (strength >= 0.9) {
        return this.flags.split("|").contains("AlwaysUseDefiniteArticle") ?
               "the " + this.name : this.indefiniteName();
    } else if (strength >= 0.8) {
        return this.gender === "male" ? "a man" : "a woman";
    } else {
        return "someone";
    }
};

Character.prototype.open = function(portal) {

    if (!portal.canOpenFromRoom(this.currentRoom)) {
        this.send("Exit cannot be opened.");
        return;
    }

    var name = portal.nameFromRoom(this.currentRoom);
    if (portal.open) {
        this.send("The %1 is already open.".arg(name));
    } else {
        if (!portal.invokeTrigger("onopen", this)) {
            return;
        }

        portal.open = true;
        this.send("You open the %1.".arg(name));

        var others = this.currentRoom.characters;
        others.removeOne(this);
        others.send("%1 opens the %2.".arg(this.definiteName(this.currentRoom.characters), name));
    }
};

Character.prototype.regenerate = function() {

    this.hp += max(Math.floor(this.stats[VITALITY] / 15), 1);
};

Character.prototype.remove = function(item) {

    if (this.weapon === item) {
        this.send("You remove your %1.".arg(item.name));
        this.addInventoryItem(item);
        this.weapon = null;
    } else if (this.secondaryWeapon === item) {
        this.send("You remove your %1.".arg(item.name));
        this.addInventoryItem(item);
        this.secondaryWeapon = null;
    } else if (this.shield === item) {
        this.send("You remove your %1.".arg(item.name));
        this.addInventoryItem(item);
        this.shield = null;
    }
};

Character.prototype.say = function(message) {

    var event = Realm.createEvent("Speech", this.currentRoom, 1.0);
    event.speaker = this;
    event.message = message;
    event.fire();

    this.setAction("talk", { "duration": 4000 });
};

Character.prototype.setAction = function(action, options) {

    options = options || {};

    this.currentAction = action;

    this.target = options.target;

    if (this.currentActionTimerId) {
        this.clearTimeout(this.currentActionTimerId);
    }

    if (options.duration) {
        this.currentActionTimerId = this.setTimeout(function() {
            this.currentAction = "";
            this.currentActionTimerId = 0;
        }, options.duration);
    } else {
        this.currentActionTimerId = 0;
    }
};

Character.prototype.shout = function(message) {

    var event = Realm.createEvent("Speech", this.currentRoom, 5.0);
    event.speaker = this;
    event.message = message;
    event.fire();

    var self = this;
    event.affectedCharacters.forEach(function(character) {
        character.invokeTrigger("onshout", self, message);
    });

    this.setAction("shout", { "duration": 4000 });
};

Character.prototype.take = function(items) {

    var room = this.currentRoom;

    var takenItems = [];
    for (var i = 0, length = items.length; i < length; i++) {
        var item = items[i];
        if (item.portable) {
            if (this.inventoryWeight() + item.weight <= this.maxInventoryWeight()) {
                if (item.name.endsWith(" worth of gold")) {
                    this.gold += item.cost;
                } else {
                    this.addInventoryItem(item);
                }
                room.removeItem(item);
                takenItems.append(item);
            } else {
                this.send("You can't take %1, because it's too heavy."
                          .arg(item.definiteName(room.items)));
            }
        } else {
            this.send("You can't take %1.".arg(item.definiteName(room.items)));
        }
    }

    if (!takenItems.isEmpty()) {
        var description = takenItems.joinFancy(Options.DefiniteArticles);
        this.send("You take %2.".arg(description));

        var others = room.characters;
        others.removeOne(this);
        others.send("%1 takes %2.".arg(this.definiteName(room.characters, Options.Capitalized),
                                       description));
    }
};

Character.prototype.talk = function(character, message) {

    var event = Realm.createEvent("Speech", this.currentRoom, 1.0);
    event.speaker = this;
    event.message = message;
    event.target = character;
    event.fire();

    if (!character.isPlayer()) {
        character.invokeTrigger("ontalk", this, message);

        if (this.isPlayer()) {
            LogUtil.logNpcTalk(character.name, message);
        }
    }

    this.setAction("talk", { "duration": 4000 });
};

Character.prototype.tell = function(player, message) {

    if (message.isEmpty()) {
        this.send("Tell %1 what?".arg(player.name));
        return;
    }

    player.send("%1 tells you, \"%2\".".arg(this.name, message));
    this.send("You tell %1, \"%2\".".arg(player.name, message));
};

Character.prototype.wield = function(item) {

    var inventory = this.inventory;
    if (!inventory.contains(item)) {
        return;
    }

    if (item.isWeapon()) {
        if (this.weapon === null) {
            this.send("You wield your %1.".arg(item.name));
            this.weapon = item;
        } else {
            if (this.characterClass.name === "wanderer" && this.secondaryWeapon === null) {
                this.send("You wield your %1 as secondary weapon.".arg(item.name));
                this.secondaryWeapon = item;
            } else if (this.weapon.name === item.name) {
                this.send("You swap your %1 for another %2.".arg(this.weapon.name, item.name));
                this.weapon = item;
            } else {
                this.send("You remove your %1 and wield your %2.".arg(this.weapon.name, item.name));
                this.weapon = item;
            }
        }
        inventory.removeOne(item);
        this.inventory = inventory;
    } else if (item.isShield()) {
        if (this.shield === null) {
            this.send("You wield your %1.".arg(item.name));
            this.shield = item;
        } else {
            this.send("You remove your %1 and wield your %2.".arg(this.shield.name, item.name));
            this.shield = item;
        }
        inventory.removeOne(item);
        this.inventory = inventory;
    } else {
        this.send("You cannot wield that.");
    }
};
