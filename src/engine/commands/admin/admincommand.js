
function AdminCommand() {
    Command.call(this);
}

AdminCommand.prototype = new Command();
AdminCommand.prototype.constructor = AdminCommand;

AdminCommand.prototype.objectsByDescription = function(description, pool) {

    if (description.name === "room") {
        return [ this._currentRoom ];
    }

    if (description.name.startsWith("#")) {
        var objectId = description.name.mid(1).toInt();
        if (!isNaN(objectId)) {
            var objects = [];
            if (description.position < 2) {
                objects.append(Realm.getObject("Unknown", objectId));
            }
            return objects;
        }
    }

    return Command.prototype.objectsByDescription.call(this, description, pool);
};
