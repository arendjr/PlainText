/*global define:false, require:false*/
define(["lib/zepto"], function($) {

    "use strict";

    function PortalDeleteDialog() {

        this.element = null;

        this.init();
    }

    PortalDeleteDialog.prototype.init = function() {

        this.element = $(`<div class="portal-delete dialog" style="display: none">
            <div>
                <p>Do you want to delete access to the portal from the current room, or delete the
                    entire portal from both directions?</p>
            </div>
            <div class="button-list">
                <button class="delete-one-button">Delete Access</button>
                <button class="delete-both-button">Delete Entire Portal</button>
                <button class="cancel-button">Cancel</button>
            </div>
        </div>`).appendTo(document.body);

        this.attachListeners();
    };

    PortalDeleteDialog.prototype.attachListeners = function() {

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

    PortalDeleteDialog.prototype.show = function(options) {

        this.options = options || {};

        this.element.show();
    };

    PortalDeleteDialog.prototype.deleteOne = function() {

        if (this.options.ondeleteone) {
            this.options.ondeleteone();
        }
    };

    PortalDeleteDialog.prototype.deleteBoth = function() {

        if (this.options.ondeleteboth) {
            this.options.ondeleteboth();
        }
    };

    PortalDeleteDialog.prototype.close = function() {

        this.element.hide();
    };

    return PortalDeleteDialog;
});
