
function ExitEditor() {

    this.element = element(".exit-editor");

    this.orderedDirections = ["north", "northeast", "east", "southeast",
                              "south", "southwest", "west", "northwest"];

    this.exit = null;

    this.options = {};

    this.init();
}

ExitEditor.prototype.init = function() {

    this.attachListeners();
};

ExitEditor.prototype.attachListeners = function() {

    var self = this;

    this.element.querySelector(".cancel-button").addEventListener("click", function() {
        self.close();
    }, false);

    this.element.querySelector(".submit-button").addEventListener("click", function() {
        self.save();
    }, false);
};

ExitEditor.prototype.add = function(options) {

    this.edit({ "name": "" }, options);
};

ExitEditor.prototype.edit = function(exit, options) {

    this.exit = exit;
    this.options = options || {};

    var directionSelect = this.element.querySelector(".direction");
    directionSelect.innerHTML = "<option></option>";
    orderedDirections.forEach(function(direction) {
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

    this.element.show();
};

ExitEditor.prototype.save = function() {

    this.exit.name = this.element.querySelector(".name").value;

    if (this.options.onsave) {
        this.options.onsave(this.exit);
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
