function MapEditor(element) {

    this.element = element;

    this.initialized = false;

    this.map = null;
    this.view = null;

    this.selectedRoomElement = null;

    this.exitEditor = null;
    this.zoomSlider = null;
    this.perspectiveSlider = null;

    this.selectedRoomId = 0;

    this.perspective = 0;

    var self = this;

    loadScript("admin/map.model.js", function() {
        self.map = new MapModel();

        if (self.view !== null) {
            self.view.setModel(self.map);
        }
    });

    loadScript("admin/map.view.js", function() {
        self.view = new MapView(element.querySelector(".canvas"));

        self.view.addSelectionListener(function(selectedRoomId) {
            self.selectedRoomId = selectedRoomId;

            self.onRoomSelectionChanged();
        });

        if (self.map !== null) {
            self.view.setModel(self.map);
        }
    });

    loadScript("admin/exiteditor.js", function() {
        self.exitEditor = new ExitEditor();
    });

    loadScript("admin/slider.widget.js", function() {
        self.zoomSlider = new SliderWidget(element.querySelector(".zoom.slider"), {
            "width": 200,
            "initialValue": 0.5
        });
        self.perspectiveSlider = new SliderWidget(element.querySelector(".perspective.slider"), {
            "width": 400
        });
    });
}

MapEditor.prototype.init = function() {

    this.selectedRoomElement = this.element.querySelector(".selected-room");

    this.attachListeners();
};

MapEditor.prototype.attachListeners = function() {

    var self = this;

    this.map.addChangeListener(function() {
        if (self.selectedRoomId !== 0) {
            var room = self.map.rooms[self.selectedRoomId];
            self.selectedRoomElement.querySelector(".name").textContent = room.name;
            self.selectedRoomElement.querySelector(".description").textContent = room.description;
            self.selectedRoomElement.querySelector(".x").value = room.x;
            self.selectedRoomElement.querySelector(".y").value = room.y;
            self.selectedRoomElement.querySelector(".z").value = room.z;
        }
    });

    this.element.querySelector(".plot.altitude").addEventListener("click", function() {
        self.plotAltitude();
    }, false);

    this.element.querySelector(".plot.roomvisit").addEventListener("click", function() {
        self.plotStats("roomvisit");
    }, false);

    this.element.querySelector(".plot.playerdeath").addEventListener("click", function() {
        self.plotStats("playerdeath");
    }, false);

    this.element.querySelector(".close").addEventListener("click", function() {
        self.close();
    }, false);

    this.selectedRoomElement.querySelector(".edit.name").addEventListener("click", function() {
        if (self.selectedRoomId) {
            controller.propertyEditor.edit("#" + self.selectedRoomId + " name", {
                "onsave": function(value) {
                    self.map.setRoomProperty(self.selectedRoomId, "name", value);
                    controller.propertyEditor.close();
                }
            });
        }
    }, false);

    this.selectedRoomElement.querySelector(".edit.description").addEventListener("click",
        function() {
        if (self.selectedRoomId) {
            controller.propertyEditor.edit("#" + self.selectedRoomId + " description", {
                "onsave": function(value) {
                    self.map.setRoomProperty(self.selectedRoomId, "description", value);
                    controller.propertyEditor.close();
                }
            });
        }
    }, false);

    this.selectedRoomElement.querySelector(".add.exit").addEventListener("click", function() {

    }, false);

    this.selectedRoomElement.querySelector(".x").addEventListener("change", function(event) {
        self.map.setRoomProperty(self.selectedRoomId, "x", event.target.value);
    });

    this.selectedRoomElement.querySelector(".y").addEventListener("change", function(event) {
        self.map.setRoomProperty(self.selectedRoomId, "y", event.target.value);
    });

    this.selectedRoomElement.querySelector(".z").addEventListener("change", function(event) {
        self.map.setRoomProperty(self.selectedRoomId, "z", event.target.value);
    });

    this.zoomSlider.element.addEventListener("change", function(event) {
        self.view.setZoom(2 * event.detail.value);
    });

    this.perspectiveSlider.element.addEventListener("change", function(event) {
        self.view.setPerspective(event.detail.value);
    });

    this.initialized = true;
};

MapEditor.prototype.open = function() {

    if (!this.initialized) {
        this.init();
    }

    this.element.show();

    this.map.load();
};

MapEditor.prototype.close = function() {

    this.element.hide();
};

MapEditor.prototype.onRoomSelectionChanged = function() {

    if (!this.selectedRoomId) {
        this.selectedRoomElement.hide();
        return;
    }

    var room = this.map.rooms[this.selectedRoomId];

    this.selectedRoomElement.querySelector(".id").textContent = room.id;
    this.selectedRoomElement.querySelector(".x").value = room.x;
    this.selectedRoomElement.querySelector(".y").value = room.y;
    this.selectedRoomElement.querySelector(".z").value = room.z;
    this.selectedRoomElement.querySelector(".name").textContent = room.name;
    this.selectedRoomElement.querySelector(".description").textContent = room.description;

    var self = this;

    var exitsSpan = this.selectedRoomElement.querySelector(".exits");
    exitsSpan.innerHTML = "";
    room.exits.forEach(function(exit) {
        if (exitsSpan.innerHTML.length) {
            exitsSpan.appendChild(document.createTextNode(", "));
        }

        var exitSpan = document.createElement("a");
        exitSpan.setAttribute("class", "exit");
        exitSpan.setAttribute("href", "javascript:void(0)");
        exitSpan.textContent = exit.name;
        exitsSpan.appendChild(exitSpan);

        exitSpan.addEventListener("click", function() {
            self.exitEditor.edit(exit, {
                "onsave": function(exit) {
                    self.map.setExit(exit);
                    self.exitEditor.close();
                }
            });
        }, false);
    });

    this.selectedRoomElement.show();
};

MapEditor.prototype.plotStats = function(type) {

    var self = this;

    controller.sendApiCall("log-retrieve stats " + type + " 100", function(data) {
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

scriptLoaded("mapeditor.js");
