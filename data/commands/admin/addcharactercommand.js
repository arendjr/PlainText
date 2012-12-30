function AddCharacterCommand() {
    AdminCommand.call(this);

    this.setDescription("Add a character to the current room.\n" +
                        "\n" +
                        "Usage: add-character <character-name>");
}

AddCharacterCommand.prototype = new AdminCommand();
AddCharacterCommand.prototype.constructor = AddCharacterCommand;

AddCharacterCommand.prototype.execute = function(player, command) {

    this.prepareExecute(player, command);

    if (!this.assertWordsLeft("Usage: add-character <character-name>")) {
        return;
    }

    var characterName = this.takeWord();

    var character = Realm.createObject("Character");
    character.name = characterName;
    character.currentRoom = this.currentRoom;
    this.currentRoom.addCharacter(character);

    this.send("Character %1 added.", characterName);
};

CommandRegistry.registerAdminCommand("add-character", new AddCharacterCommand());
