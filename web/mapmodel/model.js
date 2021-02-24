/*global define:false, require:false*/
define(["controller", "lib/laces"], function(Controller, Laces) {

    "use strict";


    var pointerTypes = {
        "room:": "rooms",
        "portal:": "portals"
    };


    function GameObject(model, object) {

        if (object) {
            object.name = object.name || "";
            object.description = object.description || "";
        }

        Laces.Model.call(this, object, { "bindChildren": false });

        Object.defineProperty(this, "model", { "value": model });

        this.bind("change", function(event) {
            var propertyName = event.key;
            if (this.constructor.savedProperties.hasOwnProperty(propertyName)) {
                this.saveProperty(propertyName);
            }
        });
    }

    GameObject.prototype = new Laces.Model();
    GameObject.prototype.constructor = GameObject;

    GameObject.prototype.resolvePointer = function(pointer) {

        if (typeof pointer === "string") {
            for (var key in pointerTypes) {
                if (pointerTypes.hasOwnProperty(key) && pointer.startsWith(key)) {
                    var objectId = parseInt(pointer.substr(key.length), 10);
                    return this.model[pointerTypes[key]][objectId];
                }
            }
            console.log("Could not resolve pointer: " + pointer);
            return null;
        } else {
            return this.model[pointer.constructor.name.toLowerCase() + "s"][pointer.id];
        }
    };

    GameObject.prototype.resolvePointers = function(propertyNames) {

        this.holdEvents();

        for (var i = 0; i < propertyNames.length; i++) {
            var propertyName = propertyNames[i];
            if (this.hasOwnProperty(propertyName)) {
                var value = this[propertyName];
                if (value instanceof Array) {
                    for (var j = 0, length = value.length; j < length; j++) {
                        value[j] = this.resolvePointer(value[j]);
                    }
                } else {
                    if (value !== null) {
                        this[propertyName] = this.resolvePointer(value);
                    }
                }
            }
        }

        this.discardHeldEvents();
    };

    GameObject.prototype.saveProperty = function(propertyName) {

        var options = this.constructor.savedProperties[propertyName] || {};

        var value = this[propertyName];

        if (options.type === "pointer") {
            value = (value === null ? "0" : value.toPointer());
        } else if (options.type === "pointerlist") {
            var pointerList = value.map(object => object.toPointer());
            value = "[" + pointerList.join(",") + "]";
        } else if (options.type === "point") {
            value = "(" + value[0] + "," + value[1] + "," + value[2] + ")";
        }

        Controller.sendApiCall("property-set " + this.id + " " + propertyName + " " + value);
    };

    GameObject.prototype.stringify = function() {

        var object = {};
        for (var propertyName in this) {
            if (this.constructor.savedProperties.hasOwnProperty(propertyName)) {
                var options = this.constructor.savedProperties[propertyName] || {};

                var value = this[propertyName];

                if (options.type === "pointer") {
                    value = (value === null ? "0" : value.toPointer());
                } else if (options.type === "pointerlist") {
                    value = value.map(object => object.toPointer());
                } else if (options.type === "point") {
                    value = [value[0], value[1], value[2]];
                }

                object[propertyName] = value;
            }
        }
        return JSON.stringify(object);
    };

    GameObject.prototype.toPointer = function() {

        return this.constructor.name.toLowerCase() + ":" + this.id;
    };


    function Room(model, jsonString) {

        var room = JSON.parse(jsonString);
        room.portals = room.portals || [];
        room.position = room.position || [0, 0, 0];
        GameObject.call(this, model, room);

        this.set("x", room.position[0], { "type": "integer" });
        this.set("y", room.position[1], { "type": "integer" });
        this.set("z", room.position[2], { "type": "integer" });
        this.set("position", function() { return [this.x, this.y, this.z]; });

        var flags = room.flags.split("|");
        this.set("dynamicPortalDescriptions", !flags.includes("OmitDynamicPortalsFromDescription"));
        this.set("distantCharacterDescriptions",
                 !flags.includes("OmitDistantCharactersFromDescription"));
        this.set("hasWalls", flags.includes("HasWalls"));
        this.set("hasCeiling", flags.includes("HasCeiling"));
        this.set("hasFloor", flags.includes("HasFloor"));
        this.set("isRoad", flags.includes("IsRoad"));
        this.set("isRiver", flags.includes("IsRiver"));
        this.set("isRoof", flags.includes("IsRoof"));
        this.set("flags", function() {
            var array = [];
            if (!this.dynamicPortalDescriptions) {
                array.push("OmitDynamicPortalsFromDescription");
            }
            if (!this.distantCharacterDescriptions) {
                array.push("OmitDistantCharactersFromDescription");
            }
            if (this.hasWalls) {
                array.push("HasWalls");
            }
            if (this.hasCeiling) {
                array.push("HasCeiling");
            }
            if (this.hasFloor) {
                array.push("HasFloor");
            }
            if (this.isRoad) {
                array.push("IsRoad");
            }
            if (this.isRiver) {
                array.push("IsRiver");
            }
            if (this.isRoof) {
                array.push("IsRoof");
            }
            return array.join("|");
        });
    }

    Room.savedProperties = {
        "description": { "type": "string" },
        "name": { "type": "string" },
        "portals": { "type": "pointerlist" },
        "position": { "type": "point" },
        "flags": { "type": "flags" }
    };

    Room.prototype = new GameObject();
    Room.prototype.constructor = Room;


    function Portal(model, jsonString) {

        var portal = JSON.parse(jsonString);
        GameObject.call(this, model, portal);
    }

    Portal.savedProperties = {
        "description": { "type": "string" },
        "description2": { "type": "string" },
        "flags": { "type": "flags" },
        "name": { "type": "string" },
        "name2": { "type": "string" },
        "room": { "type": "pointer" },
        "room2": { "type": "pointer" }
    };

    Portal.prototype = new GameObject();
    Portal.prototype.constructor = Portal;

    Portal.prototype.nameFromRoom = function(room) {

        return room && this.room && room.id === this.room.id ? this.name : this.name2;
    }


    function MapModel() {

        Laces.Model.call(this, {
            "portals": {},
            "rooms": {}
        });

        var self = this;
        this.portals.bind("remove", function(event) {
            self.holdEvents();

            var portalId = event.key;
            var portal = event.oldValue;
            for (var roomId in self.rooms) {
                if (self.rooms.hasOwnProperty(roomId)) {
                    var room = self.rooms[roomId];
                    const index = room.portals.indexOf(portal);
                    if (index > -1) {
                        room.portals.splice(index, 1);
                    }
                }
            }

            Controller.sendApiCall("object-delete " + portalId);
            self.fireHeldEvents();
        });

        Object.defineProperty(this.portals, "save", {
            "value": function(portal) {
                var command = "portal-set " + JSON.stringify(portal);

                Controller.sendApiCall(command, function(data) {
                    self.holdEvents();

                    var portal = new Portal(self, data["portal"]);
                    self.portals.set(portal.id, portal);

                    if (data.hasOwnProperty("room")) {
                        var room = new Room(self, data["room"]);
                        self.rooms.set(room.id, room);
                        room.resolvePointers(["portals"]);
                        room.portals.forEach(function(portal) {
                            portal.resolvePointers(["room", "room2"]);
                        });
                    }

                    if (data.hasOwnProperty("room2")) {
                        var room2 = new Room(self, data["room2"]);
                        self.rooms.set(room2.id, room2);
                        room2.resolvePointers(["portals"]);
                        room2.portals.forEach(function(portal) {
                            portal.resolvePointers(["room", "room2"]);
                        });
                    }

                    self.fireHeldEvents();
                });
            }
        });
    }

    MapModel.prototype = new Laces.Model();
    MapModel.prototype.constructor = MapModel;

    MapModel.prototype.holdEvents = function() {

        Laces.Model.prototype.holdEvents.call(this);

        this.rooms.holdEvents();
        this.portals.holdEvents();
    };

    MapModel.prototype.fireHeldEvents = function() {

        this.rooms.fireHeldEvents();
        this.portals.fireHeldEvents();

        Laces.Model.prototype.fireHeldEvents.call(this);
    };

    MapModel.prototype.load = function() {

        this.holdEvents();

        var self = this;
        Controller.sendApiCall("objects-list room", function(data) {
            console.log("Got rooms");
            for (var i = 0; i < data.length; i++) {
                var room = new Room(self, data[i]);
                self.rooms.set(room.id, room);
            }

            Controller.sendApiCall("objects-list portal", function(data) {
                console.log("Got portals");
                for (var i = 0; i < data.length; i++) {
                    var portal = new Portal(self, data[i]);
                    self.portals.set(portal.id, portal);
                }

                console.log("Resolving pointers");
                for (var id in self.rooms) {
                    if (self.rooms.hasOwnProperty(id)) {
                        self.rooms[id].resolvePointers(["portals"]);
                    }
                }
                for (id in self.portals) {
                    if (self.portals.hasOwnProperty(id)) {
                        self.portals[id].resolvePointers(["room", "room2"]);
                    }
                }

                console.log("Firing held events");
                self.fireHeldEvents();
                console.log("Done");
            });
        });
    };

    return MapModel;
});
