/*global define:false, require:false*/
define(["controller", "util", "lib/laces", "lib/zepto", "text!portaleditor/portaleditor.html"],
       function(Controller, Util, Laces, $, portalEditorHtml) {

    "use strict";

    function PortalEditor() {

        this.element = null;

        this.mapModel = null;
        this.mapView = null;
        this.originalSelectionListeners = [];

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

        var tie = new Laces.Tie(this.portal, portalEditorHtml);
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

        var portal = this.portal.id ? this.portal.clone() : { "id": "new" };

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
                var distance = $("input.distance", this.element).val().toInt();
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
            "Visual": $(".visualEventMultiplier", this.element).val().toDouble() / 100,
            "Sound": $(".soundEventMultiplier", this.element).val().toDouble() / 100
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
