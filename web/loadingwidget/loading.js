import { addStyle } from "../main.js";

addStyle("loadingwidget/loading");

const loadingEl = $(`<div class="loading dialog" style="display: none">
    <p>Loading...</p>
</div>`).appendTo(document.body);

export function showLoader() {
    loadingEl.show();
}

export function hideLoader() {
    loadingEl.hide();
}
