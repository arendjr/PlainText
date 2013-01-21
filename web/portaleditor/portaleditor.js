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
        this.portal.set("room", portal.room);
        this.portal.set("room2", portal.room2 || {
            "x": portal.room.x,
            "y": portal.room.y,
            "z": portal.room.z
        });
        this.portal.set("direction", Util.isDirection(portal.name) ? portal.name : "");
        this.portal.set("flags", portal.flags || "");

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

        if (this.portal.name === "" || Util.isDirection(this.portal.name)) {
            this.portal.name = direction;
        }

        if (this.portal.name2 === "" || Util.isDirection(this.portal.name2)) {
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

        portal.name = this.portal.name;
        portal.name2 = this.portal.name2;
        portal.room = this.portal.room.id;

        if (this.portal.room2) {
            portal.room2 = this.portal.room2.id;
        } else {
            portal.room2 = "new";

            var direction = $(".direction", this.element).val();
            if (Util.isDirection(portal.direction)) {
                var distance = parseInt($(".distance", this.element).val(), 10);
                var sourcePosition = this.portal.room.position;
                var vector = Util.vectorForDirection(portal.direction);
                portal.x = sourcePosition[0] + distance * vector[0];
                portal.y = sourcePosition[1] + distance * vector[1];
                portal.z = sourcePosition[2] + distance * vector[2];
            } else {
                portal.x = this.portal.x;
                portal.y = this.portal.y;
                portal.z = this.portal.z;
            }
        }

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
