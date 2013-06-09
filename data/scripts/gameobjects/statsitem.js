
function StatsItem() {
}

StatsItem.prototype.totalStats = function() {

    var totalStats = this.stats.clone();

    if (this.modifiers) {
        this.modifiers.forEach(function(modifier) {
            totalStats.add(modifier.stats);
        });
    }

    if (this.weapon) {
        totalStats.add(this.weapon.totalStats());
    }
    if (this.secondaryWeapon) {
        totalStats.add(this.secondaryWeapon.totalStats());
    }
    if (this.shield) {
        totalStats.add(this.shield.totalStats());
    }

    if (this.inventoryWeight) {
        var dexterity = totalStats[DEXTERITY] - Math.floor(this.inventoryWeight() / 5);
        totalStats[DEXTERITY] = Math.max(dexterity, 0);
    }

    return totalStats;
};
