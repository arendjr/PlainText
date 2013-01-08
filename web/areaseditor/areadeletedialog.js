/*global define:false, require:false*/
define(["lib/hogan", "lib/zepto", "text!areaseditor/areadeletedialog.html"],
       function(Hogan, $, areaDeleteDialogHtml) {

    "use strict";

    function AreaDeleteDialog() {

        this.element = null;

        this.init();
    }

    AreaDeleteDialog.prototype.init = function() {

        var areaDeleteDialogTemplate = Hogan.compile(areaDeleteDialogHtml);
        this.element = $(areaDeleteDialogTemplate.render()).appendTo(document.body);

        this.attachListeners();
    };

    AreaDeleteDialog.prototype.attachListeners = function() {

        var self = this;

        $(".unassign-button", this.element).on("click", function() {
            self.unassign();
        });

        $(".reassign-button", this.element).on("click", function() {
            self.reassign();
        });

        $(".cancel-button", this.element).on("click", function() {
            self.close();
        });
    };

    AreaDeleteDialog.prototype.show = function(options) {

        this.options = options || {};

        this.element.show();
    };

    AreaDeleteDialog.prototype.deleteOne = function() {

        if (this.options.ondeleteone) {
            this.options.ondeleteone();
        }
    };

    AreaDeleteDialog.prototype.deleteBoth = function() {

        if (this.options.ondeleteboth) {
            this.options.ondeleteboth();
        }
    };

    AreaDeleteDialog.prototype.close = function() {

        this.element.hide();
    };

    return AreaDeleteDialog;
});
