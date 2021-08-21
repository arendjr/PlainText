import { addStyle } from "../main.js";

addStyle("dialog/dialog");
addStyle("propertyeditor/propertyeditor");

export default class PropertyEditor {
    constructor() {
        this.editor = null;

        this.options = {};

        this.element = $(
            `<div class="property-editor dialog" style="display: none">
                <textarea class="property-value"></textarea>
                <div class="button-list">
                    <button class="cancel-button">Cancel</button>
                    <button class="submit-button">Submit</button>
                </div>
            </div>`
        ).appendTo(document.body);

        this.attachListeners();
    }

    attachListeners() {
        $(".cancel-button", this.element).on("click", () => {
            this.close();
        });

        $(".submit-button", this.element).on("click", () => {
            this.save();
        });
    }

    edit(value, options = {}) {
        this.options = options;

        $(".submit-button", this.element).text(
            options.submitButtonLabel ?? "Save"
        );

        this.element.show();

        this.editor = $(".property-value", this.element);
        this.editor.val(value?.trim() ?? "");
        this.editor.focus();
    }

    save() {
        if (this.editor === null) {
            return;
        }

        var value = this.editor.val();

        if (this.options.onsave) {
            this.options.onsave(value);
        } else {
            this.close();
        }
    }

    close() {
        this.options.onclose?.();

        this.element.hide();
    }
}
