import {
    directionForVector,
    getId,
    isDirection,
    noop,
    opposingDirection,
    vectorForDirection,
} from "../util.js";
import { html, shallowEqual, useReducer, useState } from "../lib.js";
import { useRooms } from "../stores/rooms.js";

function portalWithDirection(portal, direction) {
    return {
        ...portal,
        name:
            !portal.name || isDirection(portal.name) ? direction : portal.name,
        name2:
            !portal.name2 || isDirection(portal.name2)
                ? opposingDirection(direction)
                : portal.name2,
    };
}

function reducer(state, action) {
    const { portal } = state;
    switch (action.type) {
        case "set": {
            const { key, value } = action;
            return { ...state, portal: { ...portal, [key]: value } };
        }
        case "set-direction": {
            const { direction } = action;
            return {
                ...state,
                direction: isDirection(direction) ? direction : "",
                portal: portalWithDirection(portal, direction),
            };
        }
        case "set-rooms": {
            const { room, room2 } = action;

            let { direction } = state;
            let updatedPortal = {
                ...portal,
                room: room ? `room:${room.id}` : undefined,
                room2: room2 ? `room:${room2.id}` : undefined,
            };

            if (room && room2) {
                const [x1, y1, z1] = room.position;
                const [x2, y2, z2] = room2.position;
                direction = directionForVector([x2 - x1, y2 - y1, z2 - z1]);
                updatedPortal = portalWithDirection(updatedPortal, direction);
            }

            return { ...state, direction, portal: updatedPortal };
        }
        default:
            return state;
    }
}

export function PortalEditor({
    onClose,
    onDelete,
    onSubmit,
    portal: initialPortal,
    requestSelectRoom,
}) {
    const rooms = useRooms(state => state.rooms);
    const [{ direction, portal }, dispatch] = useReducer(reducer, () => ({
        direction: isDirection(initialPortal.name) ? initialPortal.name : "",
        portal: initialPortal,
    }));
    const [newRoomDistance, setDistance] = useState(0);
    const [newRoomPosition, setPosition] = useState([0, 0, 0]);

    const flags = portal.flags.split("|").filter(Boolean);
    const onToggleFlag = flag => {
        let newFlags = flags.includes(flag)
            ? flags.filter(f => f !== flag)
            : [...flags, flag];

        if (
            !newFlags.includes("CanOpenFromSide1") &&
            !newFlags.includes("CanOpenFromSide2")
        ) {
            const disabledFlags = [
                "CanSeeThroughIfOpen",
                "CanHearThroughIfOpen",
                "CanShootThroughIfOpen",
                "CanPassThroughIfOpen",
                "IsOpen",
            ];
            newFlags = newFlags.filter(flag => !disabledFlags.includes(flag));
        }

        dispatch({ type: "set", key: "flags", value: newFlags.join("|") });
    };

    const canOpen =
        flags.includes("CanOpenFromSide1") ||
        flags.includes("CanOpenFromSide2");

    const oppositeRoomText = portal.room2
        ? `room #${getId(portal.room2)}`
        : "new room";

    const onChangeNewRoomPosition = (coordinate, value) => {
        const position = newRoomPosition.slice();
        position[coordinate.charCodeAt(0) - "x".charCodeAt(0)] =
            Number.parseInt(value);
        setPosition(position);
    };

    const setRoom = key => value =>
        dispatch({
            type: "set-rooms",
            room: rooms.get(
                key === "room" ? rooms.get(value) : getId(portal.room)
            ),
            room2: rooms.get(
                key === "room2" ? rooms.get(value) : getId(portal.room2)
            ),
        });

    const submit = () => {
        if (!portal.room2) {
            const sourceRoom = rooms.get(getId(portal.room));
            if (isDirection(direction)) {
                if (newRoomDistance === 0) {
                    return;
                }

                const [x, y, z] = sourceRoom.position;
                const vector = vectorForDirection(direction);
                portal.room2 = {
                    position: [
                        x + Math.round(newRoomDistance * vector[0]),
                        y + Math.round(newRoomDistance * vector[1]),
                        z + Math.round(newRoomDistance * vector[2]),
                    ],
                };
            } else {
                if (shallowEqual(newRoomPosition, sourceRoom.position)) {
                    return;
                }

                portal.room2 = {
                    position: newRoomPosition,
                };
            }
        }

        onSubmit(portal);
    };

    const FlagOption = ({ disabled, flag, label }) => html`
        <span class="label">
            <input
                checked=${flags.includes(flag)}
                disabled=${disabled}
                id=${"flag__" + flag}
                onChange=${() => onToggleFlag(flag)}
                type="checkbox"
            />
        </span>
        <label class="end-label" for=${"flag__" + flag}>${label}</label>
    `;

    const PlainTextInput = ({ propName }) => html`<input
        onChange=${event =>
            dispatch({
                type: "set",
                key: propName,
                value: event.target.value.trim(),
            })}
        type="text"
        value=${portal[propName]}
    />`;

    const RoomIdInput = ({ propName }) => html`<input
        class=${`${propName}-id`}
        disabled=${!!portal.id}
        onChange=${event =>
            setRoom(propName)(Number.parseInt(event.target.value))}
        onFocus=${() => requestSelectRoom().then(setRoom(propName), noop)}
        size="8"
        type="text"
        value=${getId(portal[propName]) ?? ""}
    />`;

    return html`<div class="portal-editor dialog">
        <div class="column">
            <p>
                <span class="label">Name:</span>
                <${PlainTextInput} propName="name" />
            </p>
            <p>
                <span class="label">Destination:</span>
                <${PlainTextInput} propName="destination" />
            </p>
            <p>
                <span class="label">Description:</span>
                <${PlainTextInput} propName="description" />
            </p>
            <p>
                <span class="label">Room:</span>
                #<${RoomIdInput} propName="room" />
            </p>
            ${!portal.id &&
            html`<p>
                <span class="label">Direction:</span>
                <select
                    class="direction"
                    onChange=${event =>
                        dispatch({
                            type: "set-direction",
                            direction: event.target.value,
                        })}
                    value=${direction}
                >
                    <option></option>
                    <option>north</option>
                    <option>northeast</option>
                    <option>east</option>
                    <option>southeast</option>
                    <option>south</option>
                    <option>southwest</option>
                    <option>west</option>
                    <option>northwest</option>
                    <option>up</option>
                    <option>down</option>
                </select>
            </p>`}
        </div>
        <div class="column">
            <p>
                <span class="label">Name:</span>
                <${PlainTextInput} propName="name2" />
            </p>
            <p>
                <span class="label">Destination:</span>
                <${PlainTextInput} propName="destination2" />
            </p>
            <p>
                <span class="label">Description:</span>
                <${PlainTextInput} propName="description2" />
            </p>
            <p>
                <span class="label">Room:</span>
                ${!portal.id &&
                html`<input
                        checked=${!portal.room2}
                        class="destination new"
                        name="destination"
                        type="radio"
                        value="new"
                    />
                    New room
                    <input
                        checked=${!!portal.room2}
                        class="destination id"
                        name="destination"
                        type="radio"
                        value="id"
                    />`}
                #<${RoomIdInput} propName="room2" />
            </p>
            ${!portal.room2 &&
            (direction
                ? html`<p class="distance-paragraph">
                      <span class="distance label">Distance:</span>
                      <input
                          class="distance"
                          onChange=${event =>
                              setDistance(Number.parseInt(event.target.value))}
                          type="text"
                          value=${newRoomDistance}
                      />
                  </p>`
                : html`<p class="position-paragraph">
                      <span class="label">Position:</span>
                      <input
                          class="x"
                          onChange=${event =>
                              onChangeNewRoomPosition("x", event.target.value)}
                          type="number"
                          value=${newRoomPosition[0]}
                      />,
                      <input
                          class="y"
                          onChange=${event =>
                              onChangeNewRoomPosition("y", event.target.value)}
                          type="number"
                          value=${newRoomPosition[1]}
                      />,
                      <input
                          class="z"
                          onChange=${event =>
                              onChangeNewRoomPosition("z", event.target.value)}
                          type="number"
                          value=${newRoomPosition[2]}
                      />
                  </p>`)}
        </div>
        <hr />
        <div class="column">
            <p>
                <${FlagOption}
                    flag="IsHiddenFromSide1"
                    label=${`Hidden from room #${getId(portal.room)}`}
                />
            </p>
            <p class="options">
                <${FlagOption}
                    flag="CanOpenFromSide1"
                    label=${`Can open from room #${getId(portal.room)}`}
                />
            </p>
        </div>
        <div class="column">
            <p>
                <${FlagOption}
                    flag="IsHiddenFromSide2"
                    label=${`Hidden from room ${oppositeRoomText}`}
                />
            </p>
            <p class="options">
                <${FlagOption}
                    flag="CanOpenFromSide2"
                    label=${`Can open from room ${oppositeRoomText}`}
                />
            </p>
        </div>
        <hr />
        <div
            class="help"
            title="The checkboxes on the left apply to the default state of the portal (not opened), whereas the ones on the right apply if the portal is open."
        ></div>
        <div class="column">
            <p>
                <${FlagOption} flag="CanPassThrough" label="Can pass through" />
            </p>
            <p class="options">
                <${FlagOption}
                    flag="CanSeeThrough"
                    label=${html`Can see through at
                        <input min="0" max="100" size="6" type="number" />%`}
                />
            </p>
            <p class="options">
                <${FlagOption}
                    flag="CanHearThrough"
                    label=${html`Can hear through at
                        <input min="0" max="100" size="6" type="number" />%`}
                />
            </p>
            <p class="options">
                <${FlagOption}
                    flag="CanShootThrough"
                    label="Can shoot through"
                />
            </p>
        </div>
        <div class="column">
            <p>
                <${FlagOption}
                    disabled=${!canOpen}
                    flag="CanPassThroughIfOpen"
                    label="if open"
                />
            </p>
            <p class="options">
                <${FlagOption}
                    disabled=${!canOpen}
                    flag="CanSeeThroughIfOpen"
                    label="if open"
                />
            </p>
            <p class="options">
                <${FlagOption}
                    disabled=${!canOpen}
                    flag="CanHearThroughIfOpen"
                    label="if open"
                />
            </p>
            <p class="options">
                <${FlagOption}
                    disabled=${!canOpen}
                    flag="CanShootThroughIfOpen"
                    label="if open"
                />
            </p>
        </div>
        <div class="button-list">
            ${!!portal.id &&
            html`<button class="delete-button" onClick=${onDelete}>
                Delete
            </button>`}
            <button class="cancel-button" onClick=${onClose}>Cancel</button>
            <button class="submit-button" onClick=${submit}>Save</button>
        </div>
    </div>`;
}
