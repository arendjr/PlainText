function GroupCommand() {
    Command.call(this);

    this.setDescription("Show information about the group you are in.\n" +
                        "\n" +
                        "Example: group");
}

GroupCommand.prototype = new Command();
GroupCommand.prototype.constructor = GroupCommand;

GroupCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (player.group) {
        var group = player.group;

        var leader = group.leader;
        var hpColor = (leader.hp < leader.maxHp / 4 ? Color.Maroon : Color.Silver);
        var mpColor = (leader.mp < leader.maxMp / 4 ? Color.Maroon : Color.Silver);
        this.send("Leader:  %1  %2  %3"
                  .arg(leader.name.leftJustified(20),
                       Util.colorize("%1HP".arg(leader.hp).rightJustified(5), hpColor),
                       Util.colorize("%1MP".arg(leader.mp).rightJustified(5), mpColor)));

        for (var i = 0, length = group.members.length; i < length; i++) {
            var member = group.members[i];
            hpColor = (member.hp < member.maxHp / 4 ? Color.Maroon : Color.Silver);
            mpColor = (member.mp < member.maxMp / 4 ? Color.Maroon : Color.Silver);
            this.send("Member:  %1  %2  %3"
                      .arg(member.name.leftJustified(20),
                           Util.colorize("%1HP".arg(member.hp).rightJustified(5), hpColor),
                           Util.colorize("%1MP".arg(member.mp).rightJustified(5), mpColor)));
        }
    } else {
        this.send("You are not in a group.");
    }
}

CommandRegistry.registerCommand("group", new GroupCommand());
