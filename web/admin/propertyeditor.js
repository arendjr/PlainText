
function PropertyEditor() {

    this.element = element(".property-editor");

    this.editor = null;

    this.options = {};

    this.init();
}

PropertyEditor.prototype.init = function() {

    this.editor = CodeMirror.fromTextArea(this.element.querySelector("textarea"), {
        "lineNumbers": true,
        "matchBrackets": true,
        "tabSize": 4,
        "indentUnit": 4,
        "indentWithTabs": false
    });

    loadStyle("admin/codemirror/codemirror.css");
    loadStyle("admin/codemirror/util/simple-hint.css");

    this.attachListeners();
}

PropertyEditor.prototype.attachListeners = function() {

    var self = this;

    this.element.querySelector(".cancel-button").addEventListener("click", function() {
        self.close();
    }, false);

    this.element.querySelector(".submit-button").addEventListener("click", function() {
        self.save();
    }, false);
}

PropertyEditor.prototype.edit = function(value, options) {

    this.options = options || {};

    if (this.options.mode === "javascript") {
        this.editor.setOption("mode", "javascript");
        this.editor.setOption("lineWrapping", false);
    } else {
        this.editor.setOption("mode", "null");
        this.editor.setOption("lineWrapping", true);
    }

    var submitButtonLabel = options.submitButtonLabel || "Save";
    this.element.querySelector(".submit-button").textContent = submitButtonLabel;

    this.element.show();

    this.editor.setValue(value !== undefined ? value.trimmed() : "");

    this.editor.focus();
};

PropertyEditor.prototype.save = function() {

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

    this.element.hide();
};

scriptLoaded("propertyeditor.js");
