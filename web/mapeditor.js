(function() {

    loadScript("kinetic.js");


    var self = controller;


    var mapEditor = element(".map-editor");

    var selectedRoom = mapEditor.querySelector(".selected-room");
    selectedRoom.querySelector(".x").addEventListener("change", function(event) {
        var value = event.target.value;
        self.sendApiCall("property-set #" + selectedRoomId + " x " + value);
        map.rooms[selectedRoomId].x = value;
        drawMap();
    });
    selectedRoom.querySelector(".y").addEventListener("change", function(event) {
        var value = event.target.value;
        self.sendApiCall("property-set #" + selectedRoomId + " y " + value);
        map.rooms[selectedRoomId].y = value;
        drawMap();
    });
    selectedRoom.querySelector(".z").addEventListener("change", function(event) {
        var value = event.target.value;
        self.sendApiCall("property-set #" + selectedRoomId + " z " + value);
        map.rooms[selectedRoomId].z = value;
        drawMap();
    });

    var exitEditor = element(".exit-editor");
    exitEditor.querySelector(".delete-button").addEventListener("click", function() {
        // TODO
    }, false);
    exitEditor.querySelector(".cancel-button").addEventListener("click", function() {
        exitEditor.hide();
    }, false);
    exitEditor.querySelector(".save-button").addEventListener("click", function() {
        // TODO
    }, false);


    var map = {
        rooms: {},
        exits: {}
    };

    var selectedShape = null;
    var selectedRoomId = null;


    function openMap() {

        selectedRoom.hide();

        mapEditor.show();
        mapEditor.canvas = mapEditor.querySelector(".canvas");

        if (!mapEditor.stage) {
            mapEditor.stage = new Kinetic.Stage({
                container: mapEditor.canvas,
                width: mapEditor.canvas.clientWidth,
                height: mapEditor.canvas.clientHeight,
                draggable: true
            });

            map.layer = new Kinetic.Layer();
            map.layer.on("click", function(event) {
                if (selectedShape) {
                    selectedShape.setStroke("black");
                }

                if (event.shape.getId()) {
                    selectedShape = event.shape;
                    selectedShape.setStroke("orange");

                    selectRoom(event.shape.getId());
                }

                map.layer.draw();
            });

            mapEditor.stage.add(map.layer);
        }

        loadMap();
    }

    function closeMap() {

        mapEditor.hide();
    }

    function selectRoom(roomId) {

        if (roomId) {
            var room = map.rooms[roomId];
            selectedRoom.querySelector(".id").textContent = room.id;
            selectedRoom.querySelector(".x").value = room.x;
            selectedRoom.querySelector(".y").value = room.y;
            selectedRoom.querySelector(".z").value = room.z;
            selectedRoom.querySelector(".name").textContent = room.name;
            selectedRoom.querySelector(".description").textContent = room.description;

            var exitsSpan = selectedRoom.querySelector(".exits");
            exitsSpan.innerHTML = "";
            for (var i = 0, length = room.exits.length; i < length; i++) {
                var exit = room.exits[i];
                var exitSpan = document.createElement("a");
                exitSpan.setAttribute("class", "exit");
                exitSpan.setAttribute("data-exit-id", exit.id);
                exitSpan.setAttribute("href", "javascript:void(0)");
                exitSpan.textContent = exit.name;
                exitsSpan.appendChild(exitSpan);
                exitSpan.addEventListener("click", editExit, false);

                if (i < length - 1) {
                    exitsSpan.appendChild(document.createTextNode(", "));
                }
            }

            selectedRoom.show();

            selectedRoomId = roomId;
        } else {
            selectedRoom.hide();

            selectedRoomId = null;
        }
    }


    function addExit() {

        var directionSelect = exitEditor.querySelector(".direction");
        directionSelect.innerHTML = "<option></option>";
        orderedDirections.forEach(function(direction) {
            var option = document.createElement("option");
            option.textContent = direction;
            directionSelect.appendChild(option);
        });

        exitEditor.querySelector(".name").value = "";

        exitEditor.querySelector(".delete-button").hide();

        exitEditor.show();
    }

    function editExit(event) {

        var exitId = event.target.getAttribute("data-exit-id");
        var exit = map.exits[exitId];

        var directionSelect = exitEditor.querySelector(".direction");
        directionSelect.innerHTML = "<option></option>";
        orderedDirections.forEach(function(direction) {
            var option = document.createElement("option");
            if (direction === exit.name || exit.data && exit.data.directionHint === direction) {
                option.setAttribute("selected", "selected");
            }
            option.textContent = direction;
            directionSelect.appendChild(option);
        });

        exitEditor.querySelector(".name").value = exit.name;

        exitEditor.querySelector(".delete-button").show();

        exitEditor.show();
    }

    function editProperty(propertyName, onSaved) {

        if (selectedRoomId) {
            self.propertyEditor.edit("#" + selectedRoomId + " " + propertyName, onSaved);
        }
    }


    function resolvePointer(pointer) {

        if (pointer.startsWith("room:")) {
            return map.rooms[parseInt(pointer.substr(5), 10)];
        } else if (pointer.startsWith("exit:")) {
            return map.exits[parseInt(pointer.substr(5), 10)];
        } else {
            return null;
        }
    }

    function resolvePointers(object, propertyNames) {

        propertyNames.forEach(function(propertyName) {
            if (object.contains(propertyName)) {
                var value = object[propertyName];
                if (value instanceof Array) {
                    for (var i = 0, length = value.length; i < length; i++) {
                        value[i] = resolvePointer(value[i]);
                    }
                } else {
                    object[propertyName] = resolvePointer(value);
                }
            }
        });
    }


    function loadMap() {

        self.sendApiCall("rooms-list", function(data) {
            console.log("Received rooms");

            for (var i = 0; i < data.length; i++) {
                var room = JSON.parse(data[i]);
                room.x = room.x || 0;
                room.y = room.y || 0;
                room.z = room.z || 0;
                map.rooms[room.id] = room;
            }

            self.sendApiCall("exits-list", function(data) {
                console.log("Received exits");

                for (var i = 0; i < data.length; i++) {
                    var exit = JSON.parse(data[i]);
                    map.exits[exit.id] = exit;
                }

                for (var id in map.rooms) {
                    resolvePointers(map.rooms[id], ["exits", "visibleRooms"]);
                }
                for (id in map.exits) {
                    resolvePointers(map.exits[id], ["destination", "oppositeExit"]);
                }

                drawMap();
            });
        });
    }


    var perspective = 0;

    function drawMap() {

        if (Object.keys(map.rooms).isEmpty()) {
            console.log("No rooms to draw");
            return;
        }

        var roomSize = 30;

        function drawRoom(room) {
            for (var i = 0, length = room.exits.length; i < length; i++) {
                var exit = room.exits[i];
                var points = [room.x + perspective * room.z,
                              room.y - perspective * room.z,
                              exit.destination.x + perspective * exit.destination.z,
                              exit.destination.y - perspective * exit.destination.z];
                if (exit.shape) {
                    exit.shape.setPoints(points);
                } else {
                    exit.shape = new Kinetic.Line({
                        points: points,
                        stroke: "blue",
                        strokeWidth: 2,
                        listening: false
                    });
                    map.layer.add(exit.shape);
                }
            }

            var x = room.x - roomSize / 2 + perspective * room.z;
            var y = room.y - roomSize / 2 - perspective * room.z;
            if (room.shape) {
                room.shape.setX(x);
                room.shape.setY(y);
            } else {
                room.shape = new Kinetic.Rect({
                    id: room.id,
                    x: x, y: y,
                    width: roomSize, height: roomSize,
                    stroke: "black",
                    strokeWidth: 2,
                    fill: "grey"
                });
                map.layer.add(room.shape);
            }
        }

        for (var id in map.rooms) {
            drawRoom(map.rooms[id]);
        }
        for (id in map.rooms) {
            map.rooms[id].shape.moveToTop();
        }

        map.layer.draw();
    }


    function plotStats(type) {
        self.sendApiCall("log-retrieve stats " + type + " 100", function(data) {
            console.log("Received stats");

            var maxCount = 0;
            for (var key in data) {
                var count = data[key];
                if (count > maxCount) {
                    maxCount = count;
                }
            }

            for (id in map.rooms) {
                var shape = map.rooms[id].shape;
                key = "room:" + shape.getId();
                if (data.contains(key)) {
                    count = data[key];
                    var red = Math.floor(255 * count / maxCount);
                    var blue = 255 - red;
                    shape.setFill("rgb(" + red + ", 0, " + blue + ")");
                } else {
                    shape.setFill("rgb(0, 0, 255)");
                }
            }

            mapEditor.stage.draw();
        });
    }

    function plotAltitude(type) {
        var lowest = 0;
        var highest = 0;
        for (var id in map.rooms) {
            var room = map.rooms[id];
            if (room.z > highest) {
                highest = room.z;
            } else if (room.z < lowest) {
                lowest = room.z;
            }
        }

        for (id in map.rooms) {
            var shape = map.rooms[id].shape;
            id = shape.getId();
            room = map.rooms[id];

            var red = 0, green = 0, blue = 0;
            if (room.z > 0) {
                red = Math.floor(255 * room.z / highest);
                green = 255 - red;
            } else if (room.z === 0) {
                green = 255;
            } else {
                blue = Math.floor(255 * room.z / lowest);
                green = 255 - blue;
            }

            shape.setFill("rgb(" + red + ", " + green + ", " + blue + ")");
        }

        mapEditor.stage.draw();
    }


    var perspectiveSliderHandle = mapEditor.querySelector(".perspective.slider .handle");

    function startPerspectiveSlider(event) {

        var slider = mapEditor.querySelector(".perspective.slider");
        var sliderRect = slider.getBoundingClientRect();
        var handleRect = perspectiveSliderHandle.getBoundingClientRect();
        perspectiveSliderHandle.leftX = sliderRect.left + 5;
        perspectiveSliderHandle.rightX = sliderRect.right - 5;
        perspectiveSliderHandle.currentX = Math.round((handleRect.left + handleRect.right) / 2);
        perspectiveSliderHandle.offsetX = perspectiveSliderHandle.currentX - event.clientX;

        window.addEventListener("mousemove", slidePerspective, false);
        window.addEventListener("mouseup", stopPerspectiveSlider, false);
    }

    function stopPerspectiveSlider(event) {

        window.removeEventListener("mousemove", slidePerspective, false);
        window.removeEventListener("mouseup", stopPerspectiveSlider, false);
        return false;
    }

    function slidePerspective(event) {

        var newX = event.clientX + perspectiveSliderHandle.offsetX;
        if (newX < perspectiveSliderHandle.leftX) {
            newX = perspectiveSliderHandle.leftX;
        } else if (newX > perspectiveSliderHandle.rightX) {
            newX = perspectiveSliderHandle.rightX;
        }

        var left = (newX - perspectiveSliderHandle.leftX);
        perspectiveSliderHandle.style.left = left + "px";

        perspective = left / 200;
        drawMap();

        perspectiveSliderHandle.currentX = newX;
    }


    var editMapLink = document.createElement("a");
    editMapLink.setAttribute("href", "javascript:void(0)");
    editMapLink.textContent = "Edit Map";
    editMapLink.addEventListener("click", openMap, false);

    var statusHeader = element(".status-header");
    statusHeader.appendChild(document.createTextNode(" "));
    statusHeader.appendChild(editMapLink);

    selectedRoom.querySelector(".edit.name").addEventListener("click", function() {
        editProperty("name", function(newValue) {
            map.rooms[selectedRoomId].name = newValue;
            selectedRoom.querySelector(".name").textContent = newValue;
        });
    }, false);
    selectedRoom.querySelector(".edit.description").addEventListener("click", function() {
        editProperty("description", function(newValue) {
            map.rooms[selectedRoomId].description = newValue;
            selectedRoom.querySelector(".description").textContent = newValue;
        });
    }, false);
    selectedRoom.querySelector(".add.exit").addEventListener("click", addExit, false);

    mapEditor.querySelector(".plot.altitude").addEventListener("click", function() {
        plotAltitude();
    }, false);
    mapEditor.querySelector(".plot.roomvisit").addEventListener("click", function() {
        plotStats("roomvisit");
    }, false);
    mapEditor.querySelector(".plot.playerdeath").addEventListener("click", function() {
        plotStats("playerdeath");
    }, false);

    perspectiveSliderHandle.addEventListener("mousedown", startPerspectiveSlider, false);

    mapEditor.querySelector(".close").addEventListener("click", closeMap, false);

})();
