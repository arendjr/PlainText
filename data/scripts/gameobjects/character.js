
function Character() {
}

Character.prototype.changeStats = function(newStats) {

    this.maxHp = 2 * newStats[VITALITY];
    this.maxMp = newStats[INTELLIGENCE];
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
        text += description + "\n";
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
            text += "You should think twice before attacking %2.\n".arg(this.objectPronoun);
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

Character.prototype.regenerate = function() {

    this.hp += max(Math.floor(this.stats[VITALITY] / 15), 1);
};
