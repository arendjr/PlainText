/*global define:false, require:false*/
define(["controller", "lib/codemirror/codemirror", "lib/hogan", "lib/zepto",
        "text!propertyeditor/propertyeditor.html"],
       function(Controller, CodeMirror, Hogan, $, propertyEditorHtml) {

    "use strict";

    Controller.addStyle("lib/codemirror/codemirror");
    Controller.addStyle("lib/codemirror/util/simple-hint");

    require([
        "lib/codemirror/javascript",
        "lib/codemirror/util/simple-hint",
        "lib/codemirror/util/javascript-hint"
    ]);

    function PropertyEditor() {

        this.element = null;

        this.editor = null;

        this.options = {};

        this.init();
    }

    PropertyEditor.prototype.init = function() {

        Controller.addStyle("dialog/dialog");
        Controller.addStyle("propertyeditor/propertyeditor");

        var propertyEditorTemplate = Hogan.compile(propertyEditorHtml);
        this.element = $(propertyEditorTemplate.render()).appendTo(document.body);

        this.attachListeners();
    };

    PropertyEditor.prototype.attachListeners = function() {

        var self = this;

        $(".cancel-button", this.element).on("click", function() {
            self.close();
        });

        $(".submit-button", this.element).on("click", function() {
            self.save();
        });
    };

    PropertyEditor.prototype.edit = function(value, options) {

        this.editor = CodeMirror.fromTextArea($("textarea", this.element)[0], {
            "lineNumbers": true,
            "matchBrackets": true,
            "tabSize": 4,
            "indentUnit": 4,
            "indentWithTabs": false
        });

        this.options = options || {};

        if (this.options.mode === "javascript") {
            this.editor.setOption("mode", "javascript");
            this.editor.setOption("lineWrapping", false);
        } else {
            this.editor.setOption("mode", "null");
            this.editor.setOption("lineWrapping", true);
        }

        $(".submit-button", this.element).text(options.submitButtonLabel || "Save");

        this.element.show();

        this.editor.setValue(value !== undefined ? value.trimmed() : "");

        this.editor.focus();
    };

    PropertyEditor.prototype.save = function() {

        if (this.editor === null) {
            return;
        }

        var value = this.editor.getValue();

        if (this.options.onsave) {
            this.options.onsave(value);
        } else {
            this.close();
        }
    };

    PropertyEditor.prototype.close = function() {

        if (this.options.onclose) {
            this.options.onclose();
        }

        if (this.editor) {
            this.editor.toTextArea();
            this.editor = null;
        }

        this.element.hide();
    };

    return PropertyEditor;
});
