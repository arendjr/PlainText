function EnterRoomCommand() {
    AdminCommand.call(this);

    this.setDescription("Enter a room without the need for there to be an exit to the room.\n" +
                        "\n" +
                        "Example: enter-room #1234");
}

EnterRoomCommand.prototype = new AdminCommand();
EnterRoomCommand.prototype.constructor = EnterRoomCommand;

EnterRoomCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    var room = this.takeObject([]);
    if (!this.requireSome(room, "Enter what room?")) {
        return;
    }

    player.leave(this.currentRoom);
    player.enter(room);
};

CommandRegistry.registerAdminCommand("enter-room", new EnterRoomCommand());
