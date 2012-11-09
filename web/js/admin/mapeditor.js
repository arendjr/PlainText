/*global define:false, require:false*/
define(["controller", "admin/map.model", "admin/map.view", "admin/exiteditor",
        "admin/exitdeletedialog", "admin/propertyeditor", "admin/slider.widget", "zepto"],
       function(Controller, MapModel, MapView, ExitEditor,
                ExitDeleteDialog, PropertyEditor, SliderWidget, $) {

    "use strict";

    function MapEditor() {

        this.element = $(".map-editor");

        this.map = null;
        this.view = null;

        this.exitEditor = null;
        this.exitDeleteDialog = null;
        this.propertyEditor = null;
        this.zoomSlider = null;
        this.perspectiveSlider = null;

        this.selectedRoomDiv = null;
        this.selectedRoomId = 0;

        this.init();
    }

    MapEditor.prototype.init = function() {

        this.map = new MapModel();

        this.view = new MapView($(".canvas", this.element)[0]);
        this.view.setModel(this.map);

        this.exitEditor = new ExitEditor();

        this.exitDeleteDialog = new ExitDeleteDialog();

        this.propertyEditor = new PropertyEditor();

        this.zoomSlider = new SliderWidget($(".zoom.slider", this.element)[0], {
            "width": 200,
            "initialValue": 0.5
        });
        this.perspectiveSlider = new SliderWidget($(".perspective.slider", this.element)[0], {
            "width": 400
        });

        this.selectedRoomDiv = $(".selected-room", this.element);

        this.attachListeners();
    };

    MapEditor.prototype.attachListeners = function() {

        var self = this;

        this.map.addChangeListener(function() {
            if (self.selectedRoomId) {
                self.onRoomSelectionChanged();
            }
        });

        this.view.addSelectionListener(function(selectedRoomId) {
            self.selectedRoomId = selectedRoomId;
            self.onRoomSelectionChanged();
        });

        $(".plot.altitude", this.element).on("click", function() {
            self.plotAltitude();
        });

        $(".plot.roomvisit", this.element).on("click", function() {
            self.plotStats("roomvisit");
        });

        $(".plot.playerdeath", this.element).on("click", function() {
            self.plotStats("playerdeath");
        });

        $(".close", this.element).on("click", function() {
            self.close();
        });

        $(".enter-room-button", this.selectedRoomDiv).on("click", function() {
            if (self.selectedRoomId) {
                Controller.sendCommand("enter-room #" + self.selectedRoomId);
                self.close();
            }
        });

        $(".edit.name", this.selectedRoomDiv).on("click", function() {
            if (self.selectedRoomId) {
                var room = self.map.rooms[self.selectedRoomId];
                self.propertyEditor.edit(room.name, {
                    "onsave": function(value) {
                        self.map.setRoomProperty(self.selectedRoomId, "name", value);
                        self.propertyEditor.close();
                    }
                });
            }
        }, false);

        $(".edit.description", this.selectedRoomDiv).on("click", function() {
            if (self.selectedRoomId) {
                var room = self.map.rooms[self.selectedRoomId];
                self.propertyEditor.edit(room.description, {
                    "onsave": function(value) {
                        self.map.setRoomProperty(self.selectedRoomId, "description", value);
                        self.propertyEditor.close();
                    }
                });
            }
        }, false);

        this.selectedRoomDiv.on("click", ".edit.exit", function() {
            var sourceRoom = self.map.rooms[self.selectedRoomId];
            var exit = self.map.exits[event.target.getAttribute("data-exit-id")];
            self.exitEditor.edit(sourceRoom, exit, {
                "onsave": function(exit) {
                    self.map.setExit(exit);
                    self.exitEditor.close();
                },
                "ondelete": function(exitId) {
                    if (exit.oppositeExit) {
                        self.exitDeleteDialog.show({
                            "ondeleteone": function() {
                                self.map.deleteExit(exit.id);
                                self.exitDeleteDialog.close();
                            },
                            "ondeleteboth": function() {
                                self.map.deleteExit(exit.id);
                                self.map.deleteExit(exit.oppositeExit.id);
                                self.exitDeleteDialog.close();
                            }
                        });
                    } else {
                        self.map.deleteExit(exitId);
                    }

                    self.exitEditor.close();
                }
            });
        });

        $(".add.exit", this.selectedRoomDiv).on("click", function() {
            var sourceRoom = self.map.rooms[self.selectedRoomId];
            self.exitEditor.add(sourceRoom, {
                "onsave": function(exit) {
                    self.map.setExit(exit);
                    self.exitEditor.close();
                }
            });
        }, false);

        $(".x", this.selectedRoomDiv).on("change", function(event) {
            self.map.setRoomProperty(self.selectedRoomId, "x", event.target.value);
        });

        $(".y", this.selectedRoomDiv).on("change", function(event) {
            self.map.setRoomProperty(self.selectedRoomId, "y", event.target.value);
        });

        $(".z", this.selectedRoomDiv).on("change", function(event) {
            self.map.setRoomProperty(self.selectedRoomId, "z", event.target.value);
        });

        this.zoomSlider.element.addEventListener("change", function(event) {
            self.view.setZoom(2 * event.detail.value);
        }, false);

        this.perspectiveSlider.element.addEventListener("change", function(event) {
            self.view.setPerspective(event.detail.value);
        }, false);

    };

    MapEditor.prototype.open = function() {

        this.element.show();

        this.map.load();
    };

    MapEditor.prototype.close = function() {

        this.element.hide();

        Controller.setFocus();
    };

    MapEditor.prototype.onRoomSelectionChanged = function() {

        if (!this.selectedRoomId) {
            this.selectedRoomDiv.hide();
            return;
        }

        var room = this.map.rooms[this.selectedRoomId];

        $(".id", this.selectedRoomDiv).text(room.id);
        $(".x", this.selectedRoomDiv).val(room.x);
        $(".y", this.selectedRoomDiv).val(room.y);
        $(".z", this.selectedRoomDiv).val(room.z);
        $(".name", this.selectedRoomDiv).not(".edit").text(room.name);
        $(".description", this.selectedRoomDiv).not(".edit").text(room.description);

        var exitsSpan = $(".exits", this.selectedRoomDiv);
        exitsSpan.empty();
        room.exits.forEach(function(exit) {
            if (exitsSpan.children().length) {
                exitsSpan.append(", ");
            }

            var exitSpan = $("<a />", {
                "text": exit.name,
                "class": "edit exit",
                "data-exit-id": exit.id,
                "href": ["java", "script:void(0)"].join("")
            });
            exitsSpan.append(exitSpan);
        });

        this.selectedRoomDiv.show();
    };

    MapEditor.prototype.plotStats = function(type) {

        var self = this;

        Controller.sendApiCall("log-retrieve stats " + type + " 100", function(data) {
            for (var key in data) {
                var id = key.substr(5);
                data[id] = data[key];
                delete data[key];
            }

            self.view.plotData(data);
        });
    };

    MapEditor.prototype.plotAltitude = function(type) {

        var rooms = this.map.rooms;
        var data = {};
        for (var id in rooms) {
            data[id] = rooms[id].z;
        }
        this.view.plotData(data);
    };

    return MapEditor;

});
