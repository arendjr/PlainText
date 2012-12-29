/*global define:false, require:false*/
define(["lib/hogan", "lib/zepto", "text!portaleditor/portaldeletedialog.html"],
       function(Hogan, $, portalDeleteDialogHtml) {

    "use strict";

    function PortalDeleteDialog() {

        this.element = null;

        this.init();
    }

    PortalDeleteDialog.prototype.init = function() {

        var portalDeleteDialogTemplate = Hogan.compile(portalDeleteDialogHtml);
        this.element = $(portalDeleteDialogTemplate.render()).appendTo(document.body);

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
