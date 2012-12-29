/*global define:false, require:false*/
define(["util", "lib/zepto"], function(Util, $) {

    "use strict";

    function PortalEditor() {

        this.element = $(".portal-editor");

        this.orderedDirections = ["north", "northeast", "east", "southeast",
                                  "south", "southwest", "west", "northwest",
                                  "up", "down"];

        this.mapModel = null;
        this.mapView = null;
        this.originalSelectionListeners = [];

        this.portal = null;

        this.options = {};

        this.init();
    }

    PortalEditor.prototype.init = function() {

        this.attachListeners();
    };

    PortalEditor.prototype.attachListeners = function() {

        var self = this;

        $(".direction", this.element).on("change", function() {
            self.setDirection($(".direction", self.element).val());
        });

        $(".new.destination", this.element).on("change", function() {
            self.updatePositionAndDistanceVisibility();
        });

        $(".destination.id", this.element).on("change", function() {
            self.updatePositionAndDistanceVisibility();
        });

        $(".room-id", this.element).on("focus", function() {
            self.selectRoom();
        });

        $(".room2-id", this.element).on("focus", function() {
            $(".destination.id", self.element).prop("checked", true);
            self.updatePositionAndDistanceVisibility();

            self.selectRoom2();
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

        this.portal = portal;
        this.options = options || {};

        if (this.mapView) {
            this.originalSelectionListeners = this.mapView.getSelectionListeners();
            this.mapView.removeSelectionListeners();
        }

        var directionSelect = $(".direction", this.element);
        directionSelect.html("<option></option>");
        this.orderedDirections.forEach(function(direction) {
            var option = $("<option />");
            if (direction === portal.name) {
                option.attr("selected", "selected");
            }
            option.text(direction);
            directionSelect.append(option);
        });

        $(".name", this.element).val(portal.name || "");
        $(".name2", this.element).val(portal.name2 || "");

        $(".room-id", this.element).val(portal.room ? portal.room.id : "");

        $(".delete-button", this.element).css("display", portal.id ? "" : "none");

        if (portal.room2) {
            $(".destination.id", this.element).prop("checked", true);
            $(".room2-id", this.element).val(portal.room2.id);
        } else {
            $(".new.destination", this.element).prop("checked", true);
            $(".room2-id", this.element).val("");

            $(".x", this.element).val(portal.room.position[0]);
            $(".y", this.element).val(portal.room.position[1]);
            $(".z", this.element).val(portal.room.position[2]);

            this.selectRoom2();
        }

        this.updatePositionAndDistanceVisibility();

        this.element.show();
    };

    PortalEditor.prototype.setDirection = function(direction) {

        $(".direction", this.element).val(Util.isDirection(direction) ? direction : "");

        var name = $(".name", this.element).val();
        if (name === "" || Util.isDirection(name)) {
            $(".name").val(direction);
        }

        var name2 = $(".name2", this.element).val();
        if (name2 === "" || Util.isDirection(name2)) {
            if (Util.isDirection(direction)) {
                $(".name2", this.element).val(Util.opposingDirection(direction));
            }
        }

        this.updatePositionAndDistanceVisibility();
    };

    PortalEditor.prototype.updatePositionAndDistanceVisibility = function() {

        if ($(".new.destination", this.element).prop("checked")) {
            var direction = $(".direction", this.element).val();
            if (Util.isDirection(direction)) {
                $(".distance-paragraph", this.element).show();
                $(".position-paragraph", this.element).hide();
            } else {
                $(".distance-paragraph", this.element).hide();
                $(".position-paragraph", this.element).show();
            }
        } else {
            $(".distance-paragraph", this.element).hide();
            $(".position-paragraph", this.element).hide();
        }
    };

    PortalEditor.prototype.selectRoom = function() {

        if (this.mapView) {
            var self = this;

            this.mapView.removeSelectionListeners();
            this.mapView.addSelectionListener(function(selectedRoomId) {
                $(".room-id", self.element).val(selectedRoomId);

                var room2Id = $(".room2-id", self.element).val();
                if (self.mapModel && room2Id) {
                    var room = self.mapModel.rooms[selectedRoomId];
                    var room2 = self.mapModel.rooms[room2Id];
                    self.setDirection(Util.directionForVector({
                        "x": room2.position[0] - room.position[0],
                        "y": room2.position[1] - room.position[1],
                        "z": room2.position[2] - room.position[2]
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
                $(".destination.id", self.element).prop("checked", true);
                self.updatePositionAndDistanceVisibility();

                $(".room2-id", self.element).val(selectedRoomId);

                var roomId = $(".room-id", self.element).val();
                if (self.mapModel && roomId) {
                    var room = self.mapModel.rooms[roomId];
                    var room2 = self.mapModel.rooms[selectedRoomId];
                    self.setDirection(Util.directionForVector({
                        "x": room2.position[0] - room.position[0],
                        "y": room2.position[1] - room.position[1],
                        "z": room2.position[2] - room.position[2]
                    }));
                }
            });
        }
    };

    PortalEditor.prototype.save = function() {

        var portal = {};
        portal.id = this.portal.id || "new";
        portal.name = $(".name", this.element).val();
        portal.name2 = $(".name2", this.element).val();
        portal.room = $(".room-id", this.element).val();

        if ($(".destination.id", this.element).prop("checked")) {
            portal.room2 = $(".room2-id", this.element).val();
        } else {
            portal.room2 = "new";

            var direction = $(".direction", this.element).val();
            if (Util.isDirection(direction)) {
                var distance = parseInt($(".distance", this.element).val(), 10);
                var sourcePosition = this.portal.room.position;
                var vector = Util.vectorForDirection(direction);
                portal.position = [
                    sourcePosition[0] + distance * vector[0],
                    sourcePosition[1] + distance * vector[1],
                    sourcePosition[2] + distance * vector[2]
                ];
            } else {
                portal.position = [
                    parseInt($(".x", this.element).val(), 10),
                    parseInt($(".y", this.element).val(), 10),
                    parseInt($(".z", this.element).val(), 10)
                ];
            }
        }

        if (this.options.onsave) {
            this.options.onsave(portal);
        } else {
            this.close();
        }
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

        this.element.hide();
    };

    return PortalEditor;

});