/*global define:false, require:false*/
define(["controller", "util", "lib/hogan", "lib/zepto", "text!areaseditor/areaseditor.html"],
       function(Controller, Util, Hogan, $, areasEditorHtml) {

    "use strict";

    function AreasEditor() {

        this.element = null;

        this.mapModel = null;

        this.areasEditorTemplate = Hogan.compile(areasEditorHtml);

        this.init();
    }

    AreasEditor.prototype.init = function() {

        Controller.addStyle("dialog/dialog");
        Controller.addStyle("areaseditor/areaseditor");
    };

    AreasEditor.prototype.attachListeners = function() {

        var self = this;

        this.element.on("keypress", "input.name", function(event) {
            if (event.keyCode === Util.Keys.RETURN) {
                event.preventDefault();

                var name = $(event.target).val();
                if (name === "") {
                    return;
                }

                self.mapModel.areas.save({ "id": "new", "name": name });
            }
        });
        this.element.on("click", ".add.icon", function() {
            var name = $("input.name", self.element).val();
            if (name === "") {
                $("input.name", self.element).focus();
                return;
            }

            self.mapModel.areas.save({ "id": "new", "name": name });
        });

        this.element.on("click", ".close-button", function() {
            self.close();
        });

        this.mapModel.areas.bind("change", function() {
            var areas = [];
            for (var id in this) {
                if (this.hasOwnProperty(id)) {
                    areas.append(this[id]);
                }
            }

            self.element.html(self.areasEditorTemplate.render({ "areas": areas }));
            $("input.name", self.element).focus();
        }, { "initialFire": true });
    };

    AreasEditor.prototype.setMapModel = function(mapModel) {

        this.mapModel = mapModel;
    };

    AreasEditor.prototype.show = function() {

        this.element = $("<div />").appendTo(document.body);

        this.attachListeners();
    };

    AreasEditor.prototype.close = function() {

        this.element.remove();
    };

    return AreasEditor;
});
