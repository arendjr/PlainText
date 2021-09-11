import { html, useState } from "../lib.js";

export function PropertyEditor({
    onClose,
    onSubmit,
    submitButtonLabel = "save",
    value: initialValue,
}) {
    const [value, setValue] = useState(initialValue);

    return html`<div class="property-editor dialog">
        <textarea
            class="property-value"
            onChange=${event => setValue(event.target.value)}
            value=${value}
        />
        <div class="button-list">
            <button class="cancel-button" onClick=${onClose}>Cancel</button>
            <button class="submit-button" onClick=${() => onSubmit(value)}>
                ${submitButtonLabel}
            </button>
        </div>
    </div>`;
}
