/*global define:false, require:false*/
define(["controller", "admin/map.model", "admin/map.view", "admin/portaleditor",
        "admin/portaldeletedialog", "admin/propertyeditor", "admin/slider.widget", "zepto"],
       function(Controller, MapModel, MapView, PortalEditor,
                PortalDeleteDialog, PropertyEditor, SliderWidget, $) {

    "use strict";

    function MapEditor() {

        this.element = $(".map-editor");

        this.model = null;
        this.view = null;

        this.portalEditor = null;
        this.portalDeleteDialog = null;
        this.propertyEditor = null;
        this.zoomSlider = null;
        this.perspectiveSlider = null;

        this.selectedRoomDiv = null;
        this.selectedRoomId = 0;

        this.init();
    }

    MapEditor.prototype.init = function() {

        this.model = new MapModel();

        this.view = new MapView($(".canvas", this.element)[0]);
        this.view.setModel(this.model);

        this.portalEditor = new PortalEditor();
        this.portalEditor.setMapModel(this.model);
        this.portalEditor.setMapView(this.view);

        this.portalDeleteDialog = new PortalDeleteDialog();

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

        this.model.addChangeListener(function() {
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
                var room = self.model.rooms[self.selectedRoomId];
                self.propertyEditor.edit(room.name, {
                    "onsave": function(value) {
                        self.model.setRoomProperty(self.selectedRoomId, "name", value);
                        self.propertyEditor.close();
                    }
                });
            }
        }, false);

        $(".edit.description", this.selectedRoomDiv).on("click", function() {
            if (self.selectedRoomId) {
                var room = self.model.rooms[self.selectedRoomId];
                self.propertyEditor.edit(room.description, {
                    "onsave": function(value) {
                        self.model.setRoomProperty(self.selectedRoomId, "description", value);
                        self.propertyEditor.close();
                    }
                });
            }
        }, false);

        this.selectedRoomDiv.on("click", ".edit.portal", function() {
            var portal = self.model.portals[event.target.getAttribute("data-portal-id")];
            self.portalEditor.edit(portal, {
                "onsave": function(portal) {
                    self.model.setPortal(portal);
                    self.portalEditor.close();
                },
                "ondelete": function(portalId) {
                    if (portal.room.portals.contains(portal) &&
                        portal.room2.portals.contains(portal)) {
                        self.portalDeleteDialog.show({
                            "ondeleteone": function() {
                                var sourceRoom = self.model.rooms[self.selectedRoomId];
                                var portals = sourceRoom.portals.slice(0);
                                portals.removeOne(portal);
                                self.model.setRoomProperty(sourceRoom.id, "portals", portals);
                                self.portalDeleteDialog.close();
                            },
                            "ondeleteboth": function() {
                                self.model.deletePortal(portalId);
                                self.portalDeleteDialog.close();
                            }
                        });
                    } else {
                        self.model.deletePortal(portalId);
                    }

                    self.portalEditor.close();
                }
            });
        });

        $(".add.portal", this.selectedRoomDiv).on("click", function() {
            var sourceRoom = self.model.rooms[self.selectedRoomId];
            self.portalEditor.add(sourceRoom, {
                "onsave": function(portal) {
                    self.model.setPortal(portal);
                    self.portalEditor.close();
                }
            });
        }, false);

        $(".x", this.selectedRoomDiv).on("change", function(event) {
            self.model.setRoomProperty(self.selectedRoomId, "x", event.target.value);
        });

        $(".y", this.selectedRoomDiv).on("change", function(event) {
            self.model.setRoomProperty(self.selectedRoomId, "y", event.target.value);
        });

        $(".z", this.selectedRoomDiv).on("change", function(event) {
            self.model.setRoomProperty(self.selectedRoomId, "z", event.target.value);
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

        this.model.load();
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

        var room = this.model.rooms[this.selectedRoomId];

        $(".id", this.selectedRoomDiv).text(room.id);
        $(".x", this.selectedRoomDiv).val(room.x);
        $(".y", this.selectedRoomDiv).val(room.y);
        $(".z", this.selectedRoomDiv).val(room.z);
        $(".name", this.selectedRoomDiv).not(".edit").text(room.name);
        $(".description", this.selectedRoomDiv).not(".edit").text(room.description);

        var portalsSpan = $(".portals", this.selectedRoomDiv);
        portalsSpan.empty();
        room.portals.forEach(function(portal) {
            if (portalsSpan.children().length) {
                portalsSpan.append(", ");
            }

            var portalSpan = $("<a />", {
                "text": portal.name,
                "class": "edit portal",
                "data-portal-id": portal.id,
                "href": ["java", "script:void(0)"].join("")
            });
            portalsSpan.append(portalSpan);
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

        var rooms = this.model.rooms;
        var data = {};
        for (var id in rooms) {
            data[id] = rooms[id].z;
        }
        this.view.plotData(data);
    };

    return MapEditor;

});
