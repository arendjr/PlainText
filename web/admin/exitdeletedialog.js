
function ExitDeleteDialog() {

    this.element = element(".exit-delete-dialog");

    this.init();
}

ExitDeleteDialog.prototype.init = function() {

    this.attachListeners();
};

ExitDeleteDialog.prototype.attachListeners = function() {

    var self = this;

    this.element.querySelector(".delete-one-button").addEventListener("click", function() {
        self.deleteOne();
    }, false);

    this.element.querySelector(".delete-both-button").addEventListener("click", function() {
        self.deleteBoth();
    }, false);

    this.element.querySelector(".cancel-button").addEventListener("click", function() {
        self.close();
    }, false);
};

ExitDeleteDialog.prototype.show = function(options) {

    this.options = options || {};

    this.element.show();
};

ExitDeleteDialog.prototype.deleteOne = function() {

    if (this.options.ondeleteone) {
        this.options.ondeleteone();
    }
};

ExitDeleteDialog.prototype.deleteBoth = function() {

    if (this.options.ondeleteboth) {
        this.options.ondeleteboth();
    }
};

ExitDeleteDialog.prototype.close = function() {

    this.element.hide();
};

scriptLoaded("exitdeletedialog.js");
