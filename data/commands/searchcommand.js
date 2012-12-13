function SearchCommand() {
    Command.call(this);

    this.setDescription("Search the room for secret exits. It may take multiple searches to find " +
                        "an exit.\n" +
                        "\n" +
                        "Example: search");
}

SearchCommand.prototype = new Command();
SearchCommand.prototype.constructor = SearchCommand;

SearchCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (player.secondsStunned() > 0) {
        player.send("Please wait %1 seconds.".arg(player.secondsStunned()), Color.Olive);
        return;
    }

    var stats = player.totalStats();
    var isWanderer = (player.characterClass.name === "wanderer");

    var searchSkill = min(stats[INTELLIGENCE] + stats[FAITH], 100) + (isWanderer ? 30 : 0);

    var foundPortal;
    for (var i = 0, length = this.currentRoom.portals.length; i < length; i++) {
        var portal = this.currentRoom.portals[i];
        if (portal.isHiddenFromRoom(this.currentRoom)) {
            if (byChance(searchSkill, 300)) {
                foundPortal = portal;
                break;
            }
        }
    }

    if (foundPortal) {
        this.send("You found an exit: %1.", foundPortal.nameFromRoom(this.currentRoom));
    } else {
        this.send("You didn't find anything.");
        player.stun(isWanderer ? 3000 : 4000);
    }
};

CommandRegistry.registerCommand("search", new SearchCommand());
