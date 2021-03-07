import Laces from "../lib/laces.js";
import { sendApiCall } from "../main.js";

const pointerTypes = {
    "room:": "rooms",
    "portal:": "portals",
};

class GameObject extends Laces.Model {
    constructor(model, object) {
        if (object) {
            object.name = object.name || "";
            object.description = object.description || "";
        }

        super(object, { bindChildren: false });

        Object.defineProperty(this, "model", { value: model });

        this.bind("change", function (event) {
            var propertyName = event.key;
            if (this.constructor.savedProperties.hasOwnProperty(propertyName)) {
                this.saveProperty(propertyName);
            }
        });
    }

    resolvePointer(pointer) {
        if (typeof pointer === "string") {
            for (var key in pointerTypes) {
                if (
                    pointerTypes.hasOwnProperty(key) &&
                    pointer.startsWith(key)
                ) {
                    var objectId = parseInt(pointer.substr(key.length), 10);
                    return this.model[pointerTypes[key]][objectId];
                }
            }
            console.log("Could not resolve pointer: " + pointer);
            return null;
        } else {
            return this.model[pointer.constructor.name.toLowerCase() + "s"][
                pointer.id
            ];
        }
    }

    resolvePointers(propertyNames) {
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
    }

    saveProperty(propertyName) {
        var options = this.constructor.savedProperties[propertyName] || {};

        var value = this[propertyName];

        if (options.type === "pointer") {
            value = value === null ? "0" : value.toPointer();
        } else if (options.type === "pointerlist") {
            var pointerList = value.map(object => object.toPointer());
            value = `[${pointerList.join(",")}]`;
        } else if (options.type === "point") {
            value = `(${value[0]},${value[1]},${value[2]})`;
        }

        sendApiCall(
            `property-set ${this.type}:${this.id} ${propertyName} ${value}`
        );
    }

    stringify() {
        var object = {};
        for (var propertyName in this) {
            if (this.constructor.savedProperties.hasOwnProperty(propertyName)) {
                var options =
                    this.constructor.savedProperties[propertyName] || {};

                var value = this[propertyName];

                if (options.type === "pointer") {
                    value = value === null ? "0" : value.toPointer();
                } else if (options.type === "pointerlist") {
                    value = value.map(object => object.toPointer());
                } else if (options.type === "point") {
                    value = [value[0], value[1], value[2]];
                }

                object[propertyName] = value;
            }
        }
        return JSON.stringify(object);
    }

    toPointer() {
        return this.constructor.name.toLowerCase() + ":" + this.id;
    }
}

class Room extends GameObject {
    constructor(model, data) {
        var room = data;
        room.portals = room.portals || [];
        room.position = room.position || [0, 0, 0];
        super(model, room);
        this.type = "room";

        this.holdEvents();
        this.set("x", room.position[0], { type: "integer" });
        this.set("y", room.position[1], { type: "integer" });
        this.set("z", room.position[2], { type: "integer" });
        this.set("position", function () {
            return [this.x, this.y, this.z];
        });
        this.discardHeldEvents();

        var flags = room.flags.split("|");
        this.set(
            "dynamicPortalDescriptions",
            !flags.includes("OmitDynamicPortalsFromDescription")
        );
        this.set(
            "distantCharacterDescriptions",
            !flags.includes("OmitDistantCharactersFromDescription")
        );
        this.set("hasWalls", flags.includes("HasWalls"));
        this.set("hasCeiling", flags.includes("HasCeiling"));
        this.set("hasFloor", flags.includes("HasFloor"));
        this.set("isRoad", flags.includes("IsRoad"));
        this.set("isRiver", flags.includes("IsRiver"));
        this.set("isRoof", flags.includes("IsRoof"));
        this.set("flags", function () {
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

    static savedProperties = {
        description: { type: "string" },
        name: { type: "string" },
        portals: { type: "pointerlist" },
        position: { type: "point" },
        flags: { type: "flags" },
    };
}

class Portal extends GameObject {
    constructor(model, data) {
        super(model, data);
        this.type = "portal";
    }

    static savedProperties = {
        description: { type: "string" },
        description2: { type: "string" },
        flags: { type: "flags" },
        name: { type: "string" },
        name2: { type: "string" },
        room: { type: "pointer" },
        room2: { type: "pointer" },
    };

    nameFromRoom(room) {
        return room && this.room && room.id === this.room.id
            ? this.name
            : this.name2;
    }
}

export default class MapModel extends Laces.Model {
    constructor() {
        super({ portals: {}, rooms: {} });

        this.portals.bind("remove", event => {
            this.holdEvents();

            var portalId = event.key;
            var portal = event.oldValue;
            for (var roomId in this.rooms) {
                if (this.rooms.hasOwnProperty(roomId)) {
                    var room = this.rooms[roomId];
                    const index = room.portals.indexOf(portal);
                    if (index > -1) {
                        room.portals.splice(index, 1);
                    }
                }
            }

            sendApiCall("object-delete " + portalId);
            this.fireHeldEvents();
        });

        Object.defineProperty(this.portals, "save", {
            value: function (portal) {
                var command = "portal-set " + JSON.stringify(portal);

                sendApiCall(command, function (data) {
                    self.holdEvents();

                    var portal = new Portal(self, data["portal"]);
                    self.portals.set(portal.id, portal);

                    if (data.hasOwnProperty("room")) {
                        var room = new Room(self, data["room"]);
                        self.rooms.set(room.id, room);
                        room.resolvePointers(["portals"]);
                        room.portals.forEach(function (portal) {
                            portal.resolvePointers(["room", "room2"]);
                        });
                    }

                    if (data.hasOwnProperty("room2")) {
                        var room2 = new Room(self, data["room2"]);
                        self.rooms.set(room2.id, room2);
                        room2.resolvePointers(["portals"]);
                        room2.portals.forEach(function (portal) {
                            portal.resolvePointers(["room", "room2"]);
                        });
                    }

                    self.fireHeldEvents();
                });
            },
        });
    }

    holdEvents() {
        super.holdEvents();

        this.rooms.holdEvents();
        this.portals.holdEvents();
    }

    fireHeldEvents() {
        this.rooms.fireHeldEvents();
        this.portals.fireHeldEvents();

        super.fireHeldEvents();
    }

    load() {
        this.holdEvents();

        var self = this;
        sendApiCall("objects-list room", function (data) {
            console.log("Got rooms");
            for (var i = 0; i < data.length; i++) {
                var room = new Room(self, data[i]);
                self.rooms.set(room.id, room);
            }

            sendApiCall("objects-list portal", function (data) {
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
    }
}
