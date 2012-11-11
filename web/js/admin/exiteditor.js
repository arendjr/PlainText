/*global define:false, require:false*/
define(["util", "zepto"], function(Util, $) {

    "use strict";

    function ExitEditor() {

        this.element = $(".exit-editor");

        this.orderedDirections = ["north", "northeast", "east", "southeast",
                                  "south", "southwest", "west", "northwest",
                                  "up", "down"];

        this.mapModel = null;
        this.mapView = null;
        this.originalSelectionListeners = [];

        this.sourceRoom = null;
        this.exit = null;

        this.options = {};

        this.init();
    }

    ExitEditor.prototype.init = function() {

        this.attachListeners();
    };

    ExitEditor.prototype.attachListeners = function() {

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

        $(".destination-id", this.element).on("focus", function() {
            $(".destination.id", self.element).prop("checked", true);
            self.updatePositionAndDistanceVisibility();
        });

        $(".delete-button", this.element).on("click", function() {
            self.deleteExit();
        });

        $(".cancel-button", this.element).on("click", function() {
            self.close();
        });

        $(".submit-button", this.element).on("click", function() {
            self.save();
        });
    };

    ExitEditor.prototype.setMapModel = function(mapModel) {

        this.mapModel = mapModel;
    };

    ExitEditor.prototype.setMapView = function(mapView) {

        this.mapView = mapView;
    };

    ExitEditor.prototype.add = function(sourceRoom, options) {

        this.edit(sourceRoom, { "name": "" }, options);
    };

    ExitEditor.prototype.edit = function(sourceRoom, exit, options) {

        this.sourceRoom = sourceRoom;
        this.exit = exit;
        this.options = options || {};

        var directionSelect = $(".direction", this.element);
        directionSelect.html("<option></option>");
        this.orderedDirections.forEach(function(direction) {
            var option = $("<option />");
            if (direction === exit.name) {
                option.attr("selected", "selected");
            }
            option.text(direction);
            directionSelect.append(option);
        });

        $(".name", this.element).val(exit.name);

        $(".delete-button", this.element).css("display", exit.id ? "" : "none");

        if (exit.destination) {
            $(".destination.id", this.element).prop("checked", true);
            $(".destination-id", this.element).val(exit.destination.id);
        } else {
            $(".new.destination", this.element).prop("checked", true);
            $(".destination-id", this.element).val("");

            $(".x", this.element).val(sourceRoom.position[0]);
            $(".y", this.element).val(sourceRoom.position[1]);
            $(".z", this.element).val(sourceRoom.position[2]);
        }

        if (exit.oppositeExit) {
            $(".opposite", this.element).val(exit.oppositeExit.name);
        } else {
            $(".opposite", this.element).val("");
        }

        this.updatePositionAndDistanceVisibility();

        if (this.mapView) {
            this.originalSelectionListeners = this.mapView.getSelectionListeners();
            this.mapView.removeSelectionListeners();

            var self = this;
            this.mapView.addSelectionListener(function(selectedRoomId) {
                $(".destination.id", self.element).prop("checked", true);
                $(".destination-id", self.element).val(selectedRoomId);
                self.updatePositionAndDistanceVisibility();

                if (self.mapModel) {
                    var destinationRoom = self.mapModel.rooms[selectedRoomId];
                    self.setDirection(Util.directionForVector({
                        "x": destinationRoom.position[0] - sourceRoom.position[0],
                        "y": destinationRoom.position[1] - sourceRoom.position[1],
                        "z": destinationRoom.position[2] - sourceRoom.position[2]
                    }));
                }
            });
        }

        this.element.show();
    };

    ExitEditor.prototype.setDirection = function(direction) {

        $(".direction", this.element).val(Util.isDirection(direction) ? direction : "");

        var name = $(".name", this.element).val();
        if (name === "" || Util.isDirection(name)) {
            $(".name").val(direction);
        }

        var oppositeExit = $(".opposite", this.element).val();
        if (oppositeExit === "" || Util.isDirection(oppositeExit)) {
            if (Util.isDirection(direction)) {
                $(".opposite", this.element).val(Util.opposingDirection(direction));
            }
        }

        this.updatePositionAndDistanceVisibility();
    };

    ExitEditor.prototype.updatePositionAndDistanceVisibility = function() {

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

    ExitEditor.prototype.save = function() {

        var exit = {};
        exit.id = this.exit.id || "new";
        exit.name = $(".name", this.element).val();
        exit.source = this.sourceRoom.id;

        if ($(".destination.id", this.element).prop("checked")) {
            exit.destination = $(".destination-id").val();
        } else {
            exit.destination = "new";

            var direction = $(".direction", this.element).val();
            if (Util.isDirection(direction)) {
                var distance = parseInt($(".distance", this.element).val(), 10);
                var sourcePosition = this.sourceRoom.position;
                var vector = Util.directionVector(direction);
                exit.position = [
                    sourcePosition[0] + distance * vector[0],
                    sourcePosition[1] + distance * vector[1],
                    sourcePosition[2] + distance * vector[2]
                ];
            } else {
                exit.position = [
                    parseInt($(".x", this.element).val(), 10),
                    parseInt($(".y", this.element).val(), 10),
                    parseInt($(".z", this.element).val(), 10)
                ];
            }
        }

        exit.oppositeExit = $(".opposite", this.element).val();

        if (this.options.onsave) {
            this.options.onsave(exit);
        } else {
            this.close();
        }
    };

    ExitEditor.prototype.deleteExit = function() {

        if (this.options.ondelete) {
            this.options.ondelete(this.exit.id);
        } else {
            this.close();
        }
    };

    ExitEditor.prototype.close = function() {

        if (this.options.onclose) {
            this.options.onclose();
        }

        if (this.mapView) {
            this.mapView.setSelectionListeners(this.originalSelectionListeners);
        }

        this.element.hide();
    };

    return ExitEditor;

});
