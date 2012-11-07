
function ExitEditor() {

    this.element = element(".exit-editor");

    this.orderedDirections = ["north", "northeast", "east", "southeast",
                              "south", "southwest", "west", "northwest",
                              "up", "down"];

    this.sourceId = 0;
    this.exit = null;

    this.options = {};

    this.init();
}

ExitEditor.prototype.init = function() {

    this.attachListeners();
};

ExitEditor.prototype.attachListeners = function() {

    var self = this;

    this.element.querySelector(".direction").addEventListener("change", function() {
        var direction = self.element.querySelector(".direction").value;
        var name = self.element.querySelector(".name").value;
        var oppositeExit = self.element.querySelector(".opposite").value;
        if (name === "" || Util.isDirection(name)) {
            self.element.querySelector(".name").value = direction;
        }
        if (oppositeExit === "" || Util.isDirection(oppositeExit)) {
            if (Util.isDirection(direction)) {
                self.element.querySelector(".opposite").value = "";
            }
        }
        self.updatePositionAndDistanceVisibility();
    });

    this.element.querySelector(".new.destination").addEventListener("change", function() {
        self.updatePositionAndDistanceVisibility();
    }, false);

    this.element.querySelector(".destination.id").addEventListener("change", function() {
        self.updatePositionAndDistanceVisibility();
    }, false);

    this.element.querySelector(".destination-id").addEventListener("focus", function() {
        self.element.querySelector(".destination.id").checked = true;
    }, false);

    this.element.querySelector(".delete-button").addEventListener("click", function() {
        self.deleteExit();
    }, false);

    this.element.querySelector(".cancel-button").addEventListener("click", function() {
        self.close();
    }, false);

    this.element.querySelector(".submit-button").addEventListener("click", function() {
        self.save();
    }, false);
};

ExitEditor.prototype.updatePositionAndDistanceVisibility = function() {

    if (this.element.querySelector(".new.destination").checked) {
        var direction = this.element.querySelector(".direction").value;
        if (Util.isDirection(direction)) {
            this.element.querySelector(".distance-paragraph").show();
            this.element.querySelector(".position-paragraph").hide();
        } else {
            this.element.querySelector(".distance-paragraph").hide();
            this.element.querySelector(".position-paragraph").show();
        }
    } else {
        this.element.querySelector(".distance-paragraph").hide();
        this.element.querySelector(".position-paragraph").hide();
    }
};

ExitEditor.prototype.add = function(sourceId, options) {

    this.edit(sourceId, { "name": "" }, options);
};

ExitEditor.prototype.edit = function(sourceId, exit, options) {

    this.sourceId = sourceId;
    this.exit = exit;
    this.options = options || {};

    var directionSelect = this.element.querySelector(".direction");
    directionSelect.innerHTML = "<option></option>";
    this.orderedDirections.forEach(function(direction) {
        var option = document.createElement("option");
        if (direction === exit.name) {
            option.setAttribute("selected", "selected");
        }
        option.textContent = direction;
        directionSelect.appendChild(option);
    });

    this.element.querySelector(".name").value = exit.name;

    if (exit.id) {
        this.element.querySelector(".delete-button").show();
    } else {
        this.element.querySelector(".delete-button").hide();
    }

    if (exit.destination) {
        this.element.querySelector(".destination.id").checked = true;
        this.element.querySelector(".destination-id").value = exit.destination.id;
    } else {
        this.element.querySelector(".new.destination").checked = true;
        this.element.querySelector(".destination-id").value = "";
    }

    if (exit.oppositeExit) {
        this.element.querySelector(".opposite").value = exit.oppositeExit.name;
    } else {
        this.element.querySelector(".opposite").value = "";
    }

    this.updatePositionAndDistanceVisibility();

    this.element.show();
};

ExitEditor.prototype.save = function() {

    var exit = {};
    exit.id = this.exit.id || "new";
    exit.name = this.element.querySelector(".name").value;
    exit.source = this.sourceId;

    if (this.element.querySelector(".destination.id").checked) {
        exit.destination = this.element.querySelector(".destination-id").value;
    } else {
        exit.destination = "new";
    }

    exit.oppositeExit = this.element.querySelector(".opposite").value;

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

    this.element.hide();
};

scriptLoaded("exiteditor.js");
