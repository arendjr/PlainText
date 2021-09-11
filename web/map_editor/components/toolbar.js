import { Slider } from "./slider.js";
import { closeMapEditor } from "../mod.js";
import { html } from "../lib.js";
import { useViewSettings } from "../stores/view_settings.js";

export function Toolbar({
    exportAsSvg,
    plotAltitude,
    plotPlayerDeaths,
    plotRoomVisits,
    print,
}) {
    const {
        perspective,
        showRoomNames,
        zRestriction,
        zRestrictionEnabled,
        zoom,
        setPerspective,
        setShowRoomNames,
        setZRestriction,
        setZRestrictionEnabled,
        setZoom,
    } = useViewSettings();

    return html`
        <div class="toolbar">
            <div class="label menu">
                Map
                <div class="menu-content">
                    <div class="menu-item">
                        <a class="export-as-svg" onClick=${exportAsSvg}>
                            Export as SVG
                        </a>
                    </div>
                    <div class="menu-item">
                        <a class="print" onClick=${print}> Print... </a>
                    </div>
                </div>
            </div>
            <div class="label menu">
                Options
                <div class="menu-content">
                    <div class="menu-item">
                        <a class="plot altitude" onClick=${plotAltitude}>
                            Plot altitude
                        </a>
                    </div>
                    <div class="menu-item">
                        <a class="plot roomvisit" onClick=${plotRoomVisits}>
                            Plot room visits
                        </a>
                    </div>
                    <div class="menu-item">
                        <a class="plot playerdeath" onClick=${plotPlayerDeaths}>
                            Plot player deaths
                        </a>
                    </div>
                    <div class="menu-item">
                        <input
                            checked=${showRoomNames}
                            id="toggle-room-names"
                            onChange=${() => setShowRoomNames(!showRoomNames)}
                            type="checkbox"
                        />
                        <label for="toggle-room-names"> Show room names</label>
                    </div>
                    <div class="menu-item">
                        <input
                            checked=${zRestrictionEnabled}
                            onChange=${() =>
                                setZRestrictionEnabled(!zRestrictionEnabled)}
                            id="toggle-z-restriction"
                            type="checkbox"
                        />
                        <label for="toggle-z-restriction">
                            Restrict Z-level to:
                        </label>
                        <input
                            class="z-restriction"
                            onChange=${event =>
                                setZRestriction(event.target.value)}
                            type="number"
                            value=${zRestriction}
                        />
                    </div>
                </div>
            </div>
            <div class="label">Zoom:</div>
            <${Slider}
                className="zoom"
                onChange=${setZoom}
                value=${zoom}
                width=${200}
            />
            <div class="label">Perspective:</div>
            <${Slider}
                className="zoom"
                onChange=${setPerspective}
                value=${perspective}
                width=${300}
            />
            <div class="spacer" />
            <div class="close" onClick=${closeMapEditor} />
        </div>
    `;
}
