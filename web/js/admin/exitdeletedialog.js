/*global define:false, require:false*/
define(["zepto"], function($) {

    "use strict";

    function ExitDeleteDialog() {

        this.element = $(".exit-delete-dialog");

        this.init();
    }

    ExitDeleteDialog.prototype.init = function() {

        this.attachListeners();
    };

    ExitDeleteDialog.prototype.attachListeners = function() {

        var self = this;

        $(".delete-one-button", this.element).on("click", function() {
            self.deleteOne();
        });

        $(".delete-both-button", this.element).on("click", function() {
            self.deleteBoth();
        });

        $(".cancel-button", this.element).on("click", function() {
            self.close();
        });
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

    return ExitDeleteDialog;

});
