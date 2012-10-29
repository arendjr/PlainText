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
            room.x = room.position ? room.position[0] : 0;
            room.y = room.position ? room.position[1] : 0;
            room.z = room.position ? room.position[2] : 0;
            room.exits = room.exits || [];

            if (self.rooms.hasOwnProperty(room.id)) {
                room.shape = self.rooms[room.id].shape;
            }

            self.rooms[room.id] = room;
        }

        controller.sendApiCall("exits-list", function(data) {
            for (var i = 0; i < data.length; i++) {
                var exit = JSON.parse(data[i]);

                if (self.exits.hasOwnProperty(exit.id)) {
                    exit.shape = self.exits[exit.id].shape;
                }

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
    var room = this.rooms[roomId];

    if (!room.position) {
        room.position = [ 0, 0, 0 ];
    }

    if (propertyName === "x" || propertyName === "y" || propertyName === "z") {
        value = parseInt(value, 10);

        if (propertyName === "x") {
            room.x = value;
        } else if (propertyName === "y") {
            room.y = value;
        } else {
            room.z = value;
        }

        propertyName = "position";
        value = "[" + room.x + "," + room.y + "," + room.z + "]";
    }

    controller.sendApiCall("property-set #" + roomId + " " + propertyName +
                           " " + value, function() {
        room[propertyName] = value;
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
