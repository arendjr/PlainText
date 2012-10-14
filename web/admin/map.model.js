function MapModel() {

    this.rooms = {};
    this.exits = {};

    this.changeListeners = [];
}

MapModel.prototype.addChangeListener = function(listener) {

    this.changeListeners.insert(listener);
};

MapModel.prototype.removeChangeListener = function(listener) {

    this.changeListeners.removeAll(listener);
};

MapModel.prototype.notifyChangeListeners = function() {

    this.changeListeners.forEach(function(listener) {
        listener();
    });
};

MapModel.prototype.load = function() {

    var self = this;

    controller.sendApiCall("rooms-list", function(data) {
        for (var i = 0; i < data.length; i++) {
            var room = JSON.parse(data[i]);
            room.x = room.x || 0;
            room.y = room.y || 0;
            room.z = room.z || 0;
            self.rooms[room.id] = room;
        }

        controller.sendApiCall("exits-list", function(data) {
            for (var i = 0; i < data.length; i++) {
                var exit = JSON.parse(data[i]);
                self.exits[exit.id] = exit;
            }

            for (var id in self.rooms) {
                self.resolvePointers(self.rooms[id], ["exits", "visibleRooms"]);
            }
            for (id in self.exits) {
                self.resolvePointers(self.exits[id], ["destination", "oppositeExit"]);
            }

            self.notifyChangeListeners();
        });
    });
};

MapModel.prototype.setRoomProperty = function(roomId, propertyName, value) {

    var self = this;

    controller.sendApiCall("property-set #" + roomId + " " + propertyName +
                           " " + value, function() {
        self.rooms[roomId][propertyName] = value;
        self.notifyChangeListeners();
    });
}

MapModel.prototype.setExit = function(exit) {

    this.exits[exit.id] = exit;
    this.notifyChangeListeners();
}

MapModel.prototype.resolvePointer = function(pointer) {

    if (pointer.startsWith("room:")) {
        return this.rooms[parseInt(pointer.substr(5), 10)];
    } else if (pointer.startsWith("exit:")) {
        return this.exits[parseInt(pointer.substr(5), 10)];
    } else {
        return null;
    }
};

MapModel.prototype.resolvePointers = function(object, propertyNames) {

    var self = this;

    propertyNames.forEach(function(propertyName) {
        if (object.contains(propertyName)) {
            var value = object[propertyName];
            if (value instanceof Array) {
                for (var i = 0, length = value.length; i < length; i++) {
                    value[i] = self.resolvePointer(value[i]);
                }
            } else {
                object[propertyName] = self.resolvePointer(value);
            }
        }
    });
};

scriptLoaded("map.model.js");
