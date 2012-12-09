
function Character() {
}

Character.prototype.changeStats = function(newStats) {

    this.maxHp = 2 * newStats[VITALITY];
    this.maxMp = newStats[INTELLIGENCE];
};

Character.prototype.maxInventoryWeight = function() {

    return 20 + this.stats[STRENGTH] + Math.floor(this.stats[ENDURANCE] / 2);
};

Character.prototype.regenerate = function() {

    this.hp += max(Math.floor(this.stats[VITALITY] / 15), 1);
};
