/*global define:false, require:false*/
define(["controller", "loadingwidget/loading", "mapmodel/model", "mapeditor/mapview",
        "areaseditor/areaseditor", "portaleditor/portaleditor", "portaleditor/portaldeletedialog",
        "propertyeditor/propertyeditor", "sliderwidget/slider", "lib/hogan", "lib/laces.tie",
        "lib/zepto", "text!mapeditor/mapeditor.html", "text!mapeditor/areasmenu.html"],
       function(Controller, Loading, MapModel, MapView,
                AreasEditor, PortalEditor, PortalDeleteDialog,
                PropertyEditor, SliderWidget, Hogan, Laces,
                $, mapEditorHtml, areasMenuHtml) {

    "use strict";

    function MapEditor() {

        this.element = null;

        this.areasMenuTemplate = null;

        this.model = null;
        this.view = null;

        this.areasEditor = null;
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
        this.model.set("areasArray", function() {
            var areas = [];
            for (var id in this.map.areas) {
                if (this.map.areas.hasOwnProperty(id)) {
                    var area = this.map.areas[id].clone();
                    area.visible = self.view.isAreaVisible(area);
                    areas.append(area);
                }
            }
            return areas;
        });

        Controller.addStyle("mapeditor/mapeditor");

        var tie = new Laces.Tie(this.model, mapEditorHtml);
        document.body.appendChild(tie.render());
        this.element = $(".map-editor");

        this.areasMenuTie = new Laces.Tie(this.model, Hogan.compile(areasMenuHtml));

        this.view = new MapView($(".map-canvas", this.element));
        this.view.setModel(this.model.map);
        this.view.setZoom(initialZoom);

        // these are for debugging only
        window.model = this.model;
        window.view = this.view;

        this.areasEditor = new AreasEditor();
        this.areasEditor.setModel(this.model);

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
            if (self.model.selectedRoom) {
                $(".selected-room", self.element).show();

                var room = self.model.selectedRoom

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
            } else {
                $(".selected-room", self.element).hide();
            }
        });

        this.model.bind("change:areasArray", function() {
            var menuContent = $(".areas.menu-content", this.element)[0];
            menuContent.innerHTML = "";
            menuContent.appendChild(self.areasMenuTie.render());
        }, { "initialFire": true });

        this.view.addSelectionListener(function(selectedRoomId) {
            self.model.selectedRoom = self.model.map.rooms[selectedRoomId];
        });

        $(".export-as-svg", this.element).on("click", function() {
            self.view.exportSvg();
        });

        $(".print", this.element).on("click", function() {
            self.view.print();
        });

        $(".areas.menu", this.element).on("change", "input", function(event) {
            var areaId = $(event.target).data("area-id");
            if (areaId) {
                self.view.setAreaVisible(self.model.map.areas[areaId], event.target.checked);
            }
        });

        $(".areas.menu", this.element).on("click", ".edit.areas", function(event) {
            self.areasEditor.show();
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

        $(".description", this.element).on("dblclick", function() {
            if (self.model.selectedRoom) {
                var room = self.model.map.rooms[self.model.selectedRoom.id];
                self.propertyEditor.edit(room.description, {
                    "onsave": function(value) {
                        room.description = value;
                        self.propertyEditor.close();
                    }
                });
            }
        }, false);

        this.element.on("click", ".edit.portal", function() {
            var portal = self.model.map.portals[event.target.getAttribute("data-portal-id")];
            self.portalEditor.edit(portal, {
                "ondelete": function(portalId) {
                    if (portal.room.portals.contains(portal) &&
                        portal.room2.portals.contains(portal)) {
                        self.portalDeleteDialog.show({
                            "ondeleteone": function() {
                                var sourceRoom = self.model.map.rooms[self.selectedRoom.id];
                                var portals = sourceRoom.portals.clone();
                                portals.removeOne(portal);
                                sourceRoom.portals = portals;
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
