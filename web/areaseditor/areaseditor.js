/*global define:false, require:false*/
define(["controller", "util", "lib/hogan", "lib/laces.tie", "lib/zepto",
        "text!areaseditor/areaseditor.html"],
       function(Controller, Util, Hogan, Laces, $,
                areasEditorHtml) {

    "use strict";

    function AreasEditor() {

        this.element = null;

        this.model = null;
        this.areasEditorTie = null;

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

                self.model.map.areas.save({ "id": "new", "name": name });
            }
        });
        this.element.on("click", ".add.icon", function() {
            var name = $("input.name", self.element).val();
            if (name === "") {
                $("input.name", self.element).focus();
                return;
            }

            self.model.map.areas.save({ "id": "new", "name": name });
        });
        this.element.on("click", ".remove.icon", function() {
            var areaId = $(event.target).data("area-id").toInt();
            self.model.map.areas.remove(areaId);
        });

        this.element.on("click", ".close-button", function() {
            self.close();
        });

        this.model.bind("change:areasArray", function() {
            self.element.html("");
            self.element[0].appendChild(self.areasEditorTie.render());
            $("input.name", self.element).focus();
        }, { "initialFire": true });
    };

    AreasEditor.prototype.setModel = function(model) {

        this.model = model;
        this.areasEditorTie = new Laces.Tie(this.model, Hogan.compile(areasEditorHtml));
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
