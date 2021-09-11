import { html } from "../lib.js";
import { usePortals } from "../stores/portals.js";
import { useRooms } from "../stores/rooms.js";
import { parseRef, portalNameFromRoom } from "../util.js";

export function Sidebar({
    onAddPortal,
    onChange,
    onEditDescription,
    onEditPortal,
    onEnter,
    selectedRoomId,
}) {
    const room = useRooms(
        state => selectedRoomId && state.rooms.get(selectedRoomId)
    );
    const portals = usePortals(state => state.portals);

    if (!room) {
        return html`<div class="sidebar" />`;
    }

    const [x, y, z] = room.position;
    const onChangePosition = (coordinate, value) => {
        const position = room.position.slice();
        position[coordinate.charCodeAt(0) - "x".charCodeAt(0)] =
            Number.parseInt(value);
        onChange("position", position);
    };

    const flags = room.flags.split("|");
    const onToggleFlag = flag => {
        const newFlags = flags.includes(flag)
            ? flags.filter(f => f !== flag)
            : [...flags, flag];
        onChange("flags", newFlags.join("|"));
    };

    function FlagOption({ flag, label, title }) {
        return html` <p class="option">
            <span title=${title}>
                <input
                    checked=${flags.includes(flag)}
                    id=${"flag__" + flag}
                    onChange=${() => onToggleFlag(flag)}
                    type="checkbox"
                />
                <label for=${"flag__" + flag}>${label}</label>
            </span>
        </p>`;
    }

    return html`<div class="sidebar">
        <div class="selected-room">
            <div class="help" title="Double-click any property to edit" />
            <h2>${room.name || "Unnamed room"}</h2>
            <p>
                <span class="label">Room #${room.id}</span>
                <button
                    class="enter-room-button"
                    onClick=${onEnter}
                    title="Enter this room with your in-game character, regardless of where you currently are."
                >
                    Enter
                </button>
            </p>
            <p>
                <input
                    onChange=${event =>
                        onChangePosition("x", event.target.value)}
                    class="x"
                    type="number"
                    value=${x}
                />,
                <input
                    onChange=${event =>
                        onChangePosition("y", event.target.value)}
                    class="y"
                    type="number"
                    value=${y}
                />,
                <input
                    onChange=${event =>
                        onChangePosition("z", event.target.value)}
                    class="z"
                    type="number"
                    value=${z}
                />
            </p>
            <p>
                <span class="label">Description</span>
                ${room.description
                    ? html`<br /><span
                              class="description"
                              onDblclick=${onEditDescription}
                              >${room.description}</span
                          >`
                    : html`<a
                          class="add description"
                          onClick=${onEditDescription}
                      ></a>`}
            </p>
            <p>
                <span class="label">Portals</span>
                <span class="portals">
                    ${room.portals.map((portalRef, index) => {
                        const [type, id] = parseRef(portalRef);
                        const portal =
                            type === "portal" ? portals.get(id) : null;
                        if (!portal) {
                            return null;
                        }

                        const a = html`<a
                            class="edit portal"
                            onClick=${() => onEditPortal(portal.id)}
                            >${portalNameFromRoom(portal, room)}</a
                        >`;

                        return index > 0 ? [", ", a] : a;
                    })}
                </span>
                <a class="add portal" onClick=${onAddPortal}></a>
            </p>
            <p>
                <span class="label">Options</span>
            </p>
            <${FlagOption}
                flag="DynamicPortalDescriptions"
                label="Dynamic portals descriptions"
                title="If checked, dynamic room descriptions include information about portals."
            />
            <${FlagOption}
                flag="DistantCharacterDescriptions"
                label="Distant character descriptions"
                title="If checked, dynamic room descriptions include information about characters in the distance."
            />
            <${FlagOption}
                flag="HasWalls"
                label="Has walls"
                title="Whether the room has walls or not is important for calculating line-of-sight."
            />
            <${FlagOption}
                flag="HasCeiling"
                label="Has ceiling"
                title="Whether the room has a ceiling or not is important for calculating line-of-sight."
            />
            <${FlagOption}
                flag="HasFloor"
                label="Has floor"
                title="Whether the room has a floor or not is important for calculating line-of-sight."
            />
            <${FlagOption}
                flag="IsRoad"
                label="Is road"
                title="Does this room represent (part of) a road?"
            />
            <${FlagOption}
                flag="IsRiver"
                label="Is river"
                title="Does this room represent (part of) a river?"
            />
            <${FlagOption}
                flag="IsRoof"
                label="Is roof"
                title="Does this room represent (part of) a roof?"
            />
        </div>
    </div>`;
}
