/*global define:false, require:false*/
define(["codemirror/codemirror", "zepto"],
       function(CodeMirror, $) {

    "use strict";

    function loadStyle(fileName) {

        var link = document.createElement("link");
        link.setAttribute("rel", "stylesheet");
        link.setAttribute("href", fileName);
        document.head.appendChild(link);
    }

    loadStyle("js/codemirror/codemirror.css");
    loadStyle("js/codemirror/util/simple-hint.css");

    require([
        "codemirror/javascript",
        "codemirror/util/simple-hint",
        "codemirror/util/javascript-hint"
    ]);

    function PropertyEditor() {

        this.element = $(".property-editor");

        this.editor = null;

        this.options = {};

        this.init();
    }

    PropertyEditor.prototype.init = function() {

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
