/*global define:false, require:false*/
define(["controller", "loadingwidget/loading", "mapmodel/model", "mapeditor/mapview",
        "portaleditor/portaleditor",
        "portaleditor/portaldeletedialog", "propertyeditor/propertyeditor", "sliderwidget/slider",
        "util", "lib/laces.tie", "lib/zepto",
        "text!mapeditor/mapeditor.html"],
       function(Controller, Loading, MapModel, MapView,
                PortalEditor,
                PortalDeleteDialog, PropertyEditor, SliderWidget,
                Util, Laces, $,
                mapEditorHtml) {

    "use strict";

    function MapEditor() {

        this.element = null;

        this.model = null;
        this.view = null;

        this.portalEditor = null;
        this.portalDeleteDialog = null;
        this.propertyEditor = null;
        this.zoomSlider = null;
        this.perspectiveSlider = null;

        this.init();
    }

    MapEditor.prototype.init = function() {

        var initialZoom = 0.2;

        var self = this;

        this.model = new Laces.Model();
        this.model.set("map", new MapModel());
        this.model.set("selectedRoom", null);

        Controller.addStyle("mapeditor/mapeditor");

        var tie = new Laces.Tie(this.model, mapEditorHtml);
        document.body.appendChild(tie.render());
        this.element = $(".map-editor");

        this.view = new MapView($(".map-canvas", this.element));
        this.view.setModel(this.model.map);
        this.view.setZoom(initialZoom);

        // these are for debugging only
        window.model = this.model;
        window.view = this.view;

        this.portalEditor = new PortalEditor();
        this.portalEditor.setMapModel(this.model.map);
        this.portalEditor.setMapView(this.view);

        this.portalDeleteDialog = new PortalDeleteDialog();

        this.propertyEditor = new PropertyEditor();

        this.zoomSlider = new SliderWidget($(".zoom.slider", this.element)[0], {
            "width": 200,
            "initialValue": initialZoom
        });
        this.perspectiveSlider = new SliderWidget($(".perspective.slider", this.element)[0], {
            "width": 300
        });

        this.attachListeners();
    };

    MapEditor.prototype.attachListeners = function() {

        var self = this;

        this.model.bind("change:selectedRoom", function() {
            var room = self.model.selectedRoom;
            if (room) {
                var portalsSpan = $(".portals", self.element);
                portalsSpan.empty();
                room.portals.forEach(function(portal) {
                    if (portalsSpan.children().length) {
                        portalsSpan.append(", ");
                    }

                    var portalSpan = $("<a />", {
                        "text": portal.nameFromRoom(room),
                        "class": "edit portal",
                        "data-portal-id": portal.id,
                        "href": ["java", "script:void(0)"].join("")
                    });
                    portalsSpan.append(portalSpan);
                });
            }
        });

        this.view.addSelectionListener(function(selectedRoomId) {
            self.model.selectedRoom = self.model.map.rooms[selectedRoomId];
        });

        $(".export-as-svg", this.element).on("click", function() {
            self.view.exportSvg();
        });

        $(".print", this.element).on("click", function() {
            self.view.print();
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

        $("#toggle-room-names", this.element).on("change", function(event) {
            var isChecked = $("#toggle-room-names", this.element).prop("checked");
            self.view.setDisplayRoomNames(isChecked);
        });

        $("#toggle-z-restriction,.z-restriction", this.element).on("change", function(event) {
            var isChecked = $("#toggle-z-restriction", this.element).prop("checked");
            self.view.setZRestriction(isChecked ? $(".z-restriction").val() : null);
        });

        $(".close", this.element).on("click", function() {
            self.close();
        });

        $(".enter-room-button", this.element).on("click", function() {
            if (self.model.selectedRoom) {
                Controller.sendCommand("enter-room #" + self.model.selectedRoom.id);
                self.close();
            }
        });

        function editDescription() {
            var room = self.model.selectedRoom;
            if (room) {
                self.propertyEditor.edit(room.description, {
                    "onsave": function(description) {
                        room.description = description;
                        self.propertyEditor.close();
                    }
                });
            }
        }
        $(".description", this.element).on("dblclick", editDescription);
        $(".add.description", this.element).on("click", editDescription);

        this.element.on("click", ".edit.portal", function() {
            var portal = self.model.map.portals[event.target.getAttribute("data-portal-id")];
            self.portalEditor.edit(portal, {
                "ondelete": function(portalId) {
                    if (portal.room.portals.includes(portal) &&
                        portal.room2.portals.includes(portal)) {
                        self.portalDeleteDialog.show({
                            "ondeleteone": function() {
                                var sourceRoom = self.model.map.rooms[self.selectedRoom.id];
                                sourceRoom.portals = sourceRoom.portals.filter(p => p !== portal);
                                self.portalDeleteDialog.close();
                                self.portalEditor.close();
                            },
                            "ondeleteboth": function() {
                                self.model.map.portals.remove(portalId);
                                self.portalDeleteDialog.close();
                                self.portalEditor.close();
                            }
                        });
                    } else {
                        self.model.map.portals.remove(portalId);
                    }
                }
            });
        });

        $(".add.portal", this.element).on("click", function() {
            self.portalEditor.add(self.model.selectedRoom);
        }, false);

        $(".up.arrow", this.element).on("click", function() {
            self.view.move(0, -1);
        });
        $(".right.arrow", this.element).on("click", function() {
            self.view.move(1, 0);
        });
        $(".down.arrow", this.element).on("click", function() {
            self.view.move(0, 1);
        });
        $(".left.arrow", this.element).on("click", function() {
            self.view.move(-1, 0);
        });

        $(document.body).on("keydown", function(event) {
            if (event.target.tagName === "INPUT" || event.target.tagName === "TEXTAREA") {
                return;
            }

            if (event.keyCode === Util.Keys.LEFT_ARROW) {
                self.view.move(-1, 0);
            } else if (event.keyCode === Util.Keys.UP_ARROW) {
                self.view.move(0, -1);
            } else if (event.keyCode === Util.Keys.RIGHT_ARROW) {
                self.view.move(1, 0);
            } else if (event.keyCode === Util.Keys.DOWN_ARROW) {
                self.view.move(0, 1);
            }
        });

        this.zoomSlider.element.addEventListener("change", function(event) {
            self.view.setZoom(event.detail.value);
        }, false);

        this.perspectiveSlider.element.addEventListener("change", function(event) {
            self.view.setPerspective(event.detail.value);
        }, false);
    };

    MapEditor.prototype.open = function() {

        Loading.showLoader();

        this.element.show();

        this.model.map.load();
    };

    MapEditor.prototype.close = function() {

        this.element.hide();

        Controller.setFocus();
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

        var rooms = this.model.map.rooms;
        var data = {};
        for (var id in rooms) {
            data[id] = rooms[id].z;
        }
        this.view.plotData(data);
    };

    return MapEditor;
});
