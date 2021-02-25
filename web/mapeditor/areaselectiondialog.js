/*global define:false, require:false*/
define(["controller", "util", "lib/hogan", "lib/zepto", "text!mapeditor/areaselectiondialog.html"],
       function(Controller, Util, Hogan, $, areaSelectionDialogHtml) {

    "use strict";

    function AreaSelectionDialog() {

        this.element = null;

        this.model = null;
        this.areaSelectionDialogTie = null;

        this.options = {};

        this.init();
    }

    AreaSelectionDialog.prototype.init = function() {

        Controller.addStyle("dialog/dialog");
        Controller.addStyle("mapeditor/areaselectiondialog");
    };

    AreaSelectionDialog.prototype.setModel = function(model) {

        this.model = model;
        this.areaSelectionDialogTemplate = Hogan.compile(areaSelectionDialogHtml);
    };

    AreaSelectionDialog.prototype.show = function(options) {

        this.options = options || {};

        this.element = $(this.areaSelectionDialogTemplate.render(this.model));
        this.element.appendTo(document.body);

        this.attachListeners();
    };

    AreaSelectionDialog.prototype.attachListeners = function() {

        var self = this;

        this.element.on("click", ".area", function(event) {
            if (self.options.onselect) {
                var areaId = $(event.target).closest(".area").data("area-id");
                self.options.onselect(self.model.map.areas[areaId]);
            }
        });

        this.element.on("click", ".cancel-button", function() {
            self.close();
        });
    };

    AreaSelectionDialog.prototype.close = function() {

        this.element.remove();
    };

    return AreaSelectionDialog;
});
