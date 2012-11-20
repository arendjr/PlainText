/*global define:false, require:false*/
define(["controller"], function(Controller) {

    "use strict";

    function MapModel() {

        this.rooms = {};
        this.portals = {};

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

        Controller.sendApiCall("objects-list room", function(data) {
            for (var i = 0; i < data.length; i++) {
                var room = JSON.parse(data[i]);
                room.x = room.position ? room.position[0] : 0;
                room.y = room.position ? room.position[1] : 0;
                room.z = room.position ? room.position[2] : 0;
                room.portals = room.portals || [];

                if (self.rooms.hasOwnProperty(room.id)) {
                    room.shape = self.rooms[room.id].shape;
                }

                self.rooms[room.id] = room;
            }

            Controller.sendApiCall("objects-list portal", function(data) {
                for (var i = 0; i < data.length; i++) {
                    var portal = JSON.parse(data[i]);

                    if (self.portals.hasOwnProperty(portal.id)) {
                        portal.shape = self.portals[portal.id].shape;
                    }

                    self.portals[portal.id] = portal;
                }

                for (var id in self.rooms) {
                    self.resolvePointers(self.rooms[id], ["portals"]);
                }
                for (id in self.portals) {
                    self.resolvePointers(self.portals[id], ["room", "room2"]);
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
            value = "(" + room.x + "," + room.y + "," + room.z + ")";
        }

        var resolvedValue = value;
        var pointerList = [];
        if (propertyName === "portals") {
            value.forEach(function(pointer) {
                pointerList.append("portal:" + pointer.id);
            });
            value = "[" + pointerList.join(", ") + "]";
        }

        Controller.sendApiCall("property-set #" + roomId + " " + propertyName +
                               " " + value, function() {
            room[propertyName] = resolvedValue;
            self.notifyChangeListeners();
        });
    };

    MapModel.prototype.setPortal = function(portal) {

        var self = this;

        var call = "portal-set " + portal.id + " " + portal.source + " " + portal.destination +
                   " " + portal.name + " " + portal.oppositePortal +
                   (portal.position ? " (" + portal.position.join(",") + ")" : "");

        Controller.sendApiCall(call, function(data) {
            var portal = JSON.parse(data["portal"]);
            self.portals[portal.id] = portal;

            if (data.contains("source")) {
                var source = JSON.parse(data["source"]);
                source.x = source.position ? source.position[0] : 0;
                source.y = source.position ? source.position[1] : 0;
                source.z = source.position ? source.position[2] : 0;
                source.portals = source.portals || [];
                self.rooms[source.id] = source;

                self.resolvePointers(source, ["portals"]);
            }

            if (data.contains("destination")) {
                var destination = JSON.parse(data["destination"]);
                destination.x = destination.position ? destination.position[0] : 0;
                destination.y = destination.position ? destination.position[1] : 0;
                destination.z = destination.position ? destination.position[2] : 0;
                destination.portals = destination.portals || [];
                self.rooms[destination.id] = destination;

                self.resolvePointers(destination, ["portals"]);
            }

            self.resolvePointers(portal, ["room", "room2"]);

            self.notifyChangeListeners();
        });

        this.portals[portal.id] = portal;
        this.notifyChangeListeners();
    };

    MapModel.prototype.deletePortal = function(portalId) {

        var self = this;

        Controller.sendApiCall("portal-delete " + portalId, function() {
            var portal = self.portals[portalId];
            for (var roomId in self.rooms) {
                var room = self.rooms[roomId];
                room.portals.removeOne(portal);
            }
            delete self.portals[portalId];
            self.notifyChangeListeners();
        });
    };

    MapModel.prototype.resolvePointer = function(pointer) {

        if (pointer.startsWith("room:")) {
            return this.rooms[parseInt(pointer.substr(5), 10)];
        } else if (pointer.startsWith("portal:")) {
            return this.portals[parseInt(pointer.substr(7), 10)];
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

    return MapModel;
});
