import CodeMirror from "../lib/codemirror/codemirror.js";
import "../lib/codemirror/javascript.js";
import "../lib/codemirror/util/simple-hint.js";
import "../lib/codemirror/util/javascript-hint.js";
import { addStyle } from "../main.js";

addStyle("lib/codemirror/codemirror");
addStyle("lib/codemirror/util/simple-hint");
addStyle("dialog/dialog");
addStyle("propertyeditor/propertyeditor");

export default class PropertyEditor {
    constructor() {
        this.editor = null;

        this.options = {};

        this.element = $(`<div class="property-editor dialog" style="display: none">
            <textarea></textarea>
            <div class="button-list">
                <button class="cancel-button">Cancel</button>
                <button class="submit-button">Submit</button>
            </div>
        </div>`).appendTo(document.body);

        this.attachListeners();
    }

    attachListeners() {
        var self = this;

        $(".cancel-button", this.element).on("click", function () {
            self.close();
        });

        $(".submit-button", this.element).on("click", function () {
            self.save();
        });
    }

    edit(value, options) {
        this.editor = CodeMirror.fromTextArea($("textarea", this.element)[0], {
            lineNumbers: true,
            matchBrackets: true,
            tabSize: 4,
            indentUnit: 4,
            indentWithTabs: false,
        });

        this.options = options || {};

        if (this.options.mode === "javascript") {
            this.editor.setOption("mode", "javascript");
            this.editor.setOption("lineWrapping", false);
        } else {
            this.editor.setOption("mode", "null");
            this.editor.setOption("lineWrapping", true);
        }

        $(".submit-button", this.element).text(
            options.submitButtonLabel || "Save"
        );

        this.element.show();

        this.editor.setValue(value?.trim() ?? "");

        this.editor.focus();
    }

    save() {
        if (this.editor === null) {
            return;
        }

        var value = this.editor.getValue();

        if (this.options.onsave) {
            this.options.onsave(value);
        } else {
            this.close();
        }
    }

    close() {
        if (this.options.onclose) {
            this.options.onclose();
        }

        if (this.editor) {
            this.editor.toTextArea();
            this.editor = null;
        }

        this.element.hide();
    }
}
