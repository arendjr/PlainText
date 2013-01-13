/*global define:false, require:false*/
define(["controller", "loadingwidget/loading", "mapmodel/model", "mapeditor/mapview",
        "areaseditor/areaseditor", "portaleditor/portaleditor", "portaleditor/portaldeletedialog",
        "propertyeditor/propertyeditor", "sliderwidget/slider", "lib/hogan", "lib/zepto",
        "text!mapeditor/mapeditor.html", "text!mapeditor/areasmenu.html"],
       function(Controller, Loading, MapModel, MapView,
                AreasEditor, PortalEditor, PortalDeleteDialog,
                PropertyEditor, SliderWidget, Hogan, $,
                mapEditorHtml, areasMenuHtml) {

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

        this.selectedRoomDiv = null;
        this.selectedRoomId = 0;

        this.init();
    }

    MapEditor.prototype.init = function() {

        var initialZoom = 0.2;

        this.model = new MapModel();

        Controller.addStyle("mapeditor/mapeditor");

        var mapEditorTemplate = Hogan.compile(mapEditorHtml);
        this.element = $(mapEditorTemplate.render()).appendTo(document.body);

        this.areasMenuTemplate = Hogan.compile(areasMenuHtml);

        this.view = new MapView($(".map-canvas", this.element));
        this.view.setModel(this.model);
        this.view.setZoom(initialZoom);

        // these are for debugging only
        window.model = this.model;
        window.view = this.view;

        this.areasEditor = new AreasEditor();
        this.areasEditor.setMapModel(this.model);

        this.portalEditor = new PortalEditor();
        this.portalEditor.setMapModel(this.model);
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

        this.selectedRoomDiv = $(".selected-room", this.element);

        this.attachListeners();
    };

    MapEditor.prototype.attachListeners = function() {

        var self = this;

        this.model.bind("change", function() {
            if (self.selectedRoomId) {
                self.onRoomsUpdated();
            }
        });

        this.model.areas.bind("change", function() {
            self.updateAreasMenu();
        });

        this.view.addSelectionListener(function(selectedRoomId) {
            self.selectedRoomId = selectedRoomId;
            self.onRoomsUpdated();
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
                self.view.setAreaVisible(self.model.areas[areaId], event.target.checked);
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
                        room.name = value;
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
                        room.description = value;
                        self.propertyEditor.close();
                    }
                });
            }
        }, false);

        this.selectedRoomDiv.on("click", ".edit.portal", function() {
            var portal = self.model.portals[event.target.getAttribute("data-portal-id")];
            self.portalEditor.edit(portal, {
                "ondelete": function(portalId) {
                    if (portal.room.portals.contains(portal) &&
                        portal.room2.portals.contains(portal)) {
                        self.portalDeleteDialog.show({
                            "ondeleteone": function() {
                                var sourceRoom = self.model.rooms[self.selectedRoomId];
                                var portals = sourceRoom.portals.clone();
                                portals.removeOne(portal);
                                sourceRoom.portals = portals;
                                self.portalDeleteDialog.close();
                                self.portalEditor.close();
                            },
                            "ondeleteboth": function() {
                                self.model.portals.remove(portalId);
                                self.portalDeleteDialog.close();
                                self.portalEditor.close();
                            }
                        });
                    } else {
                        self.model.portals.remove(portalId);
                    }
                }
            });
        });

        $(".add.portal", this.selectedRoomDiv).on("click", function() {
            var sourceRoom = self.model.rooms[self.selectedRoomId];
            self.portalEditor.add(sourceRoom);
        }, false);

        $(".x,.y,.z", this.selectedRoomDiv).on("change", function(event) {
            var room = self.model.rooms[self.selectedRoomId];
            room[event.target.className] = event.target.value;
        });

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

        this.model.load();
    };

    MapEditor.prototype.close = function() {

        this.element.hide();

        Controller.setFocus();
    };

    MapEditor.prototype.onRoomsUpdated = function() {

        if (!this.selectedRoomId) {
            this.selectedRoomDiv.hide();
            return;
        }

        var room = this.model.rooms[this.selectedRoomId];

        $(".id", this.selectedRoomDiv).text(room.id);
        $(".name", this.selectedRoomDiv).not(".edit").text(room.name);
        $(".x", this.selectedRoomDiv).val(room.x);
        $(".y", this.selectedRoomDiv).val(room.y);
        $(".z", this.selectedRoomDiv).val(room.z);
        $(".area", this.selectedRoomDiv).not(".edit").text(room.area ? room.area.name : "(none)");
        $(".description", this.selectedRoomDiv).not(".edit").text(room.description);

        var portalsSpan = $(".portals", this.selectedRoomDiv);
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

        this.selectedRoomDiv.show();
    };

    MapEditor.prototype.updateAreasMenu = function() {

        var areas = [];
        for (var id in this.model.areas) {
            if (this.model.areas.hasOwnProperty(id)) {
                var area = this.model.areas[id];
                area.visible = this.view.isAreaVisible(area);
                areas.append(area);
            }
        }

        var menuContent = $(".areas.menu-content", this.element);
        menuContent.html(this.areasMenuTemplate.render({ "areas": areas }));
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
