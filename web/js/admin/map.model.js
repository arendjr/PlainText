/*global define:false, require:false*/
define(["controller"], function(Controller) {

    "use strict";

    function GameObject(model) {

        this.model = model;
    }

    GameObject.prototype.resolvePointer = function(pointer) {

        if (pointer.startsWith("room:")) {
            return this.model.rooms[parseInt(pointer.substr(5), 10)];
        } else if (pointer.startsWith("portal:")) {
            return this.model.portals[parseInt(pointer.substr(7), 10)];
        } else {
            return null;
        }
    };

    GameObject.prototype.resolvePointers = function(propertyNames) {

        for (var i = 0; i < propertyNames.length; i++) {
            var propertyName = propertyNames[i];
            if (this.contains(propertyName)) {
                var value = this[propertyName];
                if (value instanceof Array) {
                    for (var j = 0, length = value.length; j < length; j++) {
                        value[j] = this.resolvePointer(value[j]);
                    }
                } else {
                    this[propertyName] = this.resolvePointer(value);
                }
            }
        }
    };


    function Area(model, jsonString) {

        GameObject.call(this, model);

        var area = JSON.parse(jsonString);
        for (var key in area) {
            this[key] = area[key];
        }
    }

    Area.prototype = new GameObject();
    Area.prototype.constructor = Area;


    function Room(model, jsonString) {

        GameObject.call(this, model);

        var room = JSON.parse(jsonString);
        for (var key in room) {
            this[key] = room[key];
        }

        this.position = this.position || [0, 0, 0];
        this.x = this.position[0];
        this.y = this.position[1];
        this.z = this.position[2];

        this.portals = this.portals || [];

        if (this.model.rooms.hasOwnProperty(this.id)) {
            this.shape = this.model.rooms[this.id].shape;
        }
    }

    Room.prototype = new GameObject();
    Room.prototype.constructor = Room;

    Room.prototype.setProperty = function(propertyName, value) {

        if (["x", "y", "z"].contains(propertyName)) {
            value = parseInt(value, 10);

            if (propertyName === "x") {
                this.x = value;
            } else if (propertyName === "y") {
                this.y = value;
            } else {
                this.z = value;
            }

            propertyName = "position";
            value = "(" + this.x + "," + this.y + "," + this.z + ")";
        }

        var resolvedValue = value;
        var pointerList = [];
        if (propertyName === "portals") {
            value.forEach(function(pointer) {
                pointerList.append("portal:" + pointer.id);
            });
            value = "[" + pointerList.join(", ") + "]";
        }

        var self = this;
        Controller.sendApiCall("property-set " + this.id + " " + propertyName + " " + value,
                               function() {
            self[propertyName] = resolvedValue;
            self.model.notifyChangeListeners();
        });
    };

    function Portal(model, jsonString) {

        GameObject.call(this, model);

        var portal = JSON.parse(jsonString);
        for (var key in portal) {
            this[key] = portal[key];
        }

        if (this.model.portals.hasOwnProperty(this.id)) {
            this.shape = this.model.portals[this.id].shape;
        }
    }

    Portal.prototype = new GameObject();
    Portal.prototype.constructor = Portal;


    function MapModel() {

        this.areas = {};
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
        Controller.sendApiCall("objects-list area", function(data) {
            for (var i = 0; i < data.length; i++) {
                var area = new Area(self, data[i]);
                self.areas[area.id] = area;
            }

            Controller.sendApiCall("objects-list room", function(data) {
                for (var i = 0; i < data.length; i++) {
                    var room = new Room(self, data[i]);
                    self.rooms[room.id] = room;
                }

                Controller.sendApiCall("objects-list portal", function(data) {
                    for (var i = 0; i < data.length; i++) {
                        var portal = new Portal(self, data[i]);
                        self.portals[portal.id] = portal;
                    }

                    for (var id in self.rooms) {
                        self.rooms[id].resolvePointers(["portals"]);
                    }
                    for (id in self.portals) {
                        self.portals[id].resolvePointers(["room", "room2"]);
                    }
                    for (id in self.areas) {
                        var area = self.areas[id];
                        area.resolvePointers(["rooms"]);
                        area.rooms.forEach(function(room) {
                            room.area = area;
                        });
                    }

                    self.notifyChangeListeners();
                });
            });
        });
    };

    MapModel.prototype.setPortal = function(portal) {

        var command = "portal-set " + portal.id + " " + portal.room + " " + portal.room2 +
                      " " + portal.name + " " + portal.name2 +
                      (portal.position ? " (" + portal.position.join(",") + ")" : "");

        var self = this;
        Controller.sendApiCall(command, function(data) {
            var portal = new Portal(self, data["portal"]);
            self.portals[portal.id] = portal;

            if (data.contains("source")) {
                var source = new Room(self, data["source"]);
                self.rooms[source.id] = source;
                source.resolvePointers(["portals"]);
            }

            if (data.contains("destination")) {
                var destination = new Room(self, data["destination"]);
                self.rooms[destination.id] = destination;
                destination.resolvePointers(["portals"]);
            }

            portal.resolvePointers(["room", "room2"]);

            self.notifyChangeListeners();
        });
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

    return MapModel;
});
