import { html } from "../lib.js";
import { useViewSettings } from "../stores/view_settings.js";

export function NavigationPad() {
    const move = useViewSettings(state => state.move);

    return html`
        <div class="pad">
            <div class="up arrow" onClick=${() => move(0, -1)} />
            <div class="right arrow" onClick=${() => move(1, 0)} />
            <div class="down arrow" onClick=${() => move(0, 1)} />
            <div class="left arrow" onClick=${() => move(-1, 0)} />
        </div>
    `;
}
