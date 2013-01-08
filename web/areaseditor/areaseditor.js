/*global define:false, require:false*/
define(["controller", "lib/hogan", "lib/zepto", "text!areaseditor/areaseditor.html"],
       function(Controller, Hogan, $, areasEditorHtml) {

    "use strict";

    function AreasEditor() {

        this.element = null;

        this.mapModel = null;

        this.init();
    }

    AreasEditor.prototype.init = function() {

        Controller.addStyle("dialog/dialog");
        Controller.addStyle("areaseditor/areaseditor");
    };

    AreasEditor.prototype.attachListeners = function() {

        var self = this;

        $(".close-button", this.element).on("click", function() {
            self.close();
        });
    };

    AreasEditor.prototype.setMapModel = function(mapModel) {

        this.mapModel = mapModel;
    };

    AreasEditor.prototype.show = function() {

        var areas = [];
        for (var id in this.mapModel.areas) {
            if (this.mapModel.areas.hasOwnProperty(id)) {
                areas.append(this.mapModel.areas[id]);
            }
        }

        var areasEditorTemplate = Hogan.compile(areasEditorHtml);
        this.element = $(areasEditorTemplate.render({ "areas": areas })).appendTo(document.body);

        this.attachListeners();
    };

    AreasEditor.prototype.close = function() {

        this.element.remove();
    };

    return AreasEditor;
});
