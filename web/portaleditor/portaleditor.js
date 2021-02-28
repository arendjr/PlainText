/*global define:false, require:false*/
define(["controller", "util", "lib/laces", "lib/zepto"],
       function(Controller, Util, Laces, $) {

    "use strict";

    function PortalEditor() {

        this.element = null;

        this.mapModel = null;
        this.mapView = null;
        this.originalSelectionListeners = new Set();

        this.portal = new Laces.Model();
        this.portal.set("room2", {});
        this.portal.set("direction", "");
        this.portal.set("distanceVisible", function() {
            return !this.room2.id && this.direction;
        });
        this.portal.set("positionVisible", function() {
            return !this.room2.id && !this.direction;
        });
        this.portal.set("oppositeRoomText", function() {
            return this.room2.id ? "room #" + this.room2.id : "new room";
        });
        this.portal.set("flags", {
            IsHiddenFromSide1: false,
            IsHiddenFromSide2: false,
            CanOpenFromSide1: false,
            CanOpenFromSide2: false,
            CanSeeThrough: false,
            CanHearThrough: false,
            CanShootThrough: false,
            CanPassThrough: false,
            CanSeeThroughIfOpen: false,
            CanHearThroughIfOpen: false,
            CanShootThroughIfOpen: false,
            CanPassThroughIfOpen: false,
            IsOpen: false
        });
        this.portal.set("canOpen", function() {
            return this.flags.CanOpenFromSide1 || this.flags.CanOpenFromSide2;
        });

        var self = this;
        this.portal.bind("change:canOpen", function(event) {
            if (!event.value) {
                self.portal.flags.CanSeeThroughIfOpen = false;
                self.portal.flags.CanHearThroughIfOpen = false;
                self.portal.flags.CanShootThroughIfOpen = false;
                self.portal.flags.CanPassThroughIfOpen = false;
                self.portal.flags.IsOpen = false;
            }
        });

        // for debugging only
        window.portal = this.portal;

        this.options = {};

        this.init();
    }

    PortalEditor.prototype.init = function() {

        Controller.addStyle("dialog/dialog");
        Controller.addStyle("portaleditor/portaleditor");

        var tie = new Laces.Tie(this.portal, `<div class="portal-editor dialog" style="display: none">
            <div class="column">
                <p>
                    <span class="label">Name:</span>
                    <input type="text" data-laces-property="name">
                </p>
                <p>
                    <span class="label">Destination:</span>
                    <input type="text" data-laces-property="destination">
                </p>
                <p>
                    <span class="label">Description:</span>
                    <input type="text" data-laces-property="description">
                </p>
                <p>
                    <span class="label">Room:</span>
                    #<input type="text" size="8" class="room-id"
                            data-laces="{ property: room.id, disabled: id }">
                </p>
                <p>
                    <span class="label">Direction:</span>
                    <select class="direction" data-laces-property="direction">
                        <option></option>
                        <option>north</option>
                        <option>northeast</option>
                        <option>east</option>
                        <option>southeast</option>
                        <option>south</option>
                        <option>southwest</option>
                        <option>west</option>
                        <option>northwest</option>
                        <option>up</option>
                        <option>down</option>
                    </select>
                </p>
            </div>
            <div class="column">
                <p>
                    <span class="label">Name:</span>
                    <input type="text" data-laces-property="name2">
                </p>
                <p>
                    <span class="label">Destination:</span>
                    <input type="text" data-laces-property="destination2">
                </p>
                <p>
                    <span class="label">Description:</span>
                    <input type="text" data-laces-property="description2">
                </p>
                <p>
                    <span class="label">Room:</span>
                    <input type="radio" name="destination" class="destination new" value="new"
                        data-laces="{ checked: !room2.id, disabled: id }">
                    New room
                    <input type="radio" name="destination" class="destination id" value="id"
                        data-laces="{ checked: room2.id, disabled: id }">
                    #<input type="text" size="8" class="room2-id"
                            data-laces="{ property: room2.id, disabled: id }">
                </p>
                <p class="distance-paragraph" data-laces-visible="distanceVisible">
                    <span class="distance label">Distance:</span>
                    <input type="text" class="distance">
                </p>
                <p class="position-paragraph" data-laces-visible="positionVisible">
                    <span class="label">Position:</span>
                    <input type="number" class="x" data-laces-property="room2.x">,
                    <input type="number" class="y" data-laces-property="room2.y">,
                    <input type="number" class="z" data-laces-property="room2.z">
                </p>
            </div>
            <hr>
            <div class="column">
                <p>
                    <span class="label">
                        <input type="checkbox" data-laces-property="flags.IsHiddenFromSide1">
                    </span>
                    <span class="end-label">Hidden from room #<span data-laces-property="room.id"></span>
                    </span>
                </p>
                <p class="options">
                    <span class="label">
                        <input type="checkbox" data-laces-property="flags.CanOpenFromSide1">
                    </span>
                    <span class="end-label">Can open from room #<span data-laces-property="room.id"></span>
                    </span>
                </p>
            </div>
            <div class="column">
                <p>
                    <span class="label">
                        <input type="checkbox" data-laces-property="flags.IsHiddenFromSide2">
                    </span>
                    <span class="end-label">Hidden from
                        <span data-laces-property="oppositeRoomText"></span>
                    </span>
                </p>
                <p class="options">
                    <span class="label">
                        <input type="checkbox" data-laces-property="flags.CanOpenFromSide2">
                    </span>
                    <span class="end-label">Can open from
                        <span data-laces-property="oppositeRoomText"></span>
                    </span>
                </p>
            </div>
            <hr>
            <div class="help" title="The checkboxes on the left apply to the default state of the portal (not opened), whereas the ones on the right apply if the portal is open."></div>
            <div class="column">
                <p>
                    <span class="label">
                        <input type="checkbox" data-laces-property="flags.CanPassThrough">
                    </span>
                    <span class="end-label">Can pass through</span>
                </p>
                <p class="options">
                    <span class="label">
                        <input type="checkbox" data-laces-property="flags.CanSeeThrough">
                    </span>
                    <span class="end-label">Can see through at
                    <input type="number" min="0" max="100" class="visualEventMultiplier">%</span>
                </p>
                <p class="options">
                    <span class="label">
                        <input type="checkbox" data-laces-property="flags.CanHearThrough">
                    </span>
                    <span class="end-label">Can hear through at
                    <input type="number" min="0" max="100" class="soundEventMultiplier">%</span>
                </p>
                <p class="options">
                    <span class="label">
                        <input type="checkbox" data-laces-property="flags.CanShootThrough">
                    </span>
                    <span class="end-label">Can shoot through</span>
                </p>
            </div>
            <div class="column">
                <p>
                    <span class="label">
                        <input type="checkbox"
                            data-laces="{ property: flags.CanPassThroughIfOpen, disabled: !canOpen }">
                    </span>
                    <span class="end-label">if open</span>
                </p>
                <p class="options">
                    <span class="label">
                        <input type="checkbox"
                            data-laces="{ property: flags.CanSeeThroughIfOpen, disabled: !canOpen }">
                    </span>
                    <span class="end-label">if open</span>
                </p>
                <p class="options">
                    <span class="label">
                        <input type="checkbox"
                            data-laces="{ property: flags.CanHearThroughIfOpen, disabled: !canOpen }">
                    </span>
                    <span class="end-label">if open</span>
                </p>
                <p class="options">
                    <span class="label">
                        <input type="checkbox"
                            data-laces="{ property: flags.CanShootThroughIfOpen, disabled: !canOpen }">
                    </span>
                    <span class="end-label">if open</span>
                </p>
            </div>
            <div class="button-list">
                <button class="delete-button" data-laces-visible="id">Delete</button>
                <button class="cancel-button">Cancel</button>
                <button class="submit-button">Save</button>
            </div>
        </div>`);
        document.body.appendChild(tie.render());
        this.element = $(".portal-editor.dialog");

        this.attachListeners();
    };

    PortalEditor.prototype.attachListeners = function() {

        var self = this;

        $(".direction", this.element).on("change", function() {
            self.setDirection($(".direction", self.element).val());
        });

        $(".room-id", this.element).on("focus", function() {
            self.selectRoom();
        });

        $(".room2-id", this.element).on("focus", function() {
            self.selectRoom2();
        });

        $(".new.destination", this.element).on("change", function() {
            if ($(this).prop("checked")) {
                self.portal.room2 = {
                    "x": self.portal.room.x,
                    "y": self.portal.room.y,
                    "z": self.portal.room.z
                };
            }
        });

        $(".delete-button", this.element).on("click", function() {
            self.deletePortal();
        });

        $(".cancel-button", this.element).on("click", function() {
            self.close();
        });

        $(".submit-button", this.element).on("click", function() {
            self.save();
        });
    };

    PortalEditor.prototype.setMapModel = function(mapModel) {

        this.mapModel = mapModel;
    };

    PortalEditor.prototype.setMapView = function(mapView) {

        this.mapView = mapView;
    };

    PortalEditor.prototype.add = function(sourceRoom, options) {

        this.edit({ "room": sourceRoom }, options);
    };

    PortalEditor.prototype.edit = function(portal, options) {

        this.portal.set("id", portal.id);
        this.portal.set("name", portal.name);
        this.portal.set("name2", portal.name2);
        this.portal.set("destination", portal.destination);
        this.portal.set("destination2", portal.destination2);
        this.portal.set("description", portal.description);
        this.portal.set("description2", portal.description2);
        this.portal.set("room", portal.room);
        this.portal.set("room2", portal.room2 || {
            "x": portal.room.x,
            "y": portal.room.y,
            "z": portal.room.z
        });
        this.portal.set("direction", Util.isDirection(portal.name) ? portal.name : "");

        var self = this;
        Object.keys(this.portal.flags).forEach(function(flag) {
            self.portal.flags.set(flag, false);
        });
        (portal.flags || "").split("|").forEach(function(flag) {
            self.portal.flags.set(flag, true);
        });

        var multipliers = portal.eventMultipliers || {};
        $(".visualEventMultiplier", this.element).val(100 * (multipliers["Visual"] || 1.0));
        $(".soundEventMultiplier", this.element).val(100 * (multipliers["Sound"] || 1.0));

        $(".distance.label", this.element).css("color", "black");

        this.options = options || {};

        if (this.mapView) {
            this.originalSelectionListeners = this.mapView.getSelectionListeners();
            this.mapView.removeSelectionListeners();
        }

        if (!portal.room2) {
            this.selectRoom2();
        }

        $(this.element).show();
    };

    PortalEditor.prototype.setDirection = function(direction) {

        this.portal.direction = Util.isDirection(direction) ? direction : "";

        if (!this.portal.name || Util.isDirection(this.portal.name)) {
            this.portal.name = direction;
        }

        if (!this.portal.name2 || Util.isDirection(this.portal.name2)) {
            this.portal.name2 = Util.opposingDirection(direction);
        }
    };

    PortalEditor.prototype.selectRoom = function() {

        if (this.mapView) {
            var self = this;

            this.mapView.removeSelectionListeners();
            this.mapView.addSelectionListener(function(selectedRoomId) {
                self.portal.room = self.mapModel.rooms[selectedRoomId];

                if (self.mapModel && self.portal.room2) {
                    var room = self.portal.room;
                    var room2 = self.portal.room2;
                    self.setDirection(Util.directionForVector({
                        "x": room2.x - room.x,
                        "y": room2.y - room.y,
                        "z": room2.z - room.z
                    }));
                }
            });
        }
    };

    PortalEditor.prototype.selectRoom2 = function() {

        if (this.mapView) {
            var self = this;

            this.mapView.removeSelectionListeners();
            this.mapView.addSelectionListener(function(selectedRoomId) {
                self.portal.room2 = self.mapModel.rooms[selectedRoomId];

                if (self.mapModel && self.portal.room) {
                    var room = self.portal.room;
                    var room2 = self.portal.room2;
                    self.setDirection(Util.directionForVector({
                        "x": room2.x - room.x,
                        "y": room2.y - room.y,
                        "z": room2.z - room.z
                    }));
                }
            });
        }
    };

    PortalEditor.prototype.save = function() {

        var portal = this.portal.id ? { ...this.portal } : { "id": "new" };

        portal.name = this.portal.name || "";
        portal.name2 = this.portal.name2 || "";
        portal.destination = this.portal.destination || "";
        portal.destination2 = this.portal.destination2 || "";
        portal.description = this.portal.description || "";
        portal.description2 = this.portal.description2 || "";

        portal.room = this.portal.room.id;

        if (this.portal.room2.id) {
            portal.room2 = this.portal.room2.id;
        } else {
            portal.room2 = "new";

            if (Util.isDirection(this.portal.direction)) {
                var distance = parseInt($("input.distance", this.element).val());
                if (distance === 0) {
                    $(".distance.label", this.element).css("color", "red");
                    return;
                }

                var sourcePosition = this.portal.room.position;
                var vector = Util.vectorForDirection(this.portal.direction);
                portal.position = [ this.portal.room.x + distance * vector[0],
                                    this.portal.room.y + distance * vector[1],
                                    this.portal.room.z + distance * vector[2] ];
            } else {
                portal.position = [ this.portal.room2.x, this.portal.room2.y, this.portal.room2.z ];
            }
        }

        var flags = [];
        var self = this;
        Object.keys(this.portal.flags).forEach(function(flag) {
            if (self.portal.flags[flag]) {
                flags.push(flag);
            }
        });
        portal.flags = flags.join("|");

        portal.eventMultipliers = {
            "Visual": parseFloat($(".visualEventMultiplier", this.element).val()) / 100,
            "Sound": parseFloat($(".soundEventMultiplier", this.element).val()) / 100
        };

        this.mapModel.portals.save(portal);

        this.close();
    };

    PortalEditor.prototype.deletePortal = function() {

        if (this.options.ondelete) {
            this.options.ondelete(this.portal.id);
        } else {
            this.close();
        }
    };

    PortalEditor.prototype.close = function() {

        if (this.options.onclose) {
            this.options.onclose();
        }

        if (this.mapView) {
            this.mapView.setSelectionListeners(this.originalSelectionListeners);
        }

        $(this.element).hide();
    };

    return PortalEditor;
});
