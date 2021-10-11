import { Keys, sendApiCall, sendCommand } from "../../main.js";
import { Loader } from "./loader.js";
import { MapView } from "./map_view.js";
import { NavigationPad } from "./navigation_pad.js";
import { PortalEditor } from "./portal_editor.js";
import { PropertyEditor } from "./property_editor.js";
import { Sidebar } from "./sidebar.js";
import { Toolbar } from "./toolbar.js";
import { formatRef } from "../util.js";
import { html, shallowEqual, useEffect, useRef, useState } from "../lib.js";
import { usePortals } from "../stores/portals.js";
import { useRooms } from "../stores/rooms.js";
import { useViewSettings } from "../stores/view_settings.js";
import { DeletePortalDialog } from "./delete_portal_dialog.js";
import { closeMapEditor } from "../mod.js";

const newPortal = {
    name: "",
    name2: "",
    description: "",
    description2: "",
    destination: "",
    destination2: "",
    flags: "",
};

export function MapEditor() {
    const move = useViewSettings(state => state.move);
    const {
        rooms,
        selectedRoomId,
        addRoom,
        setRooms,
        setSelectedRoomId,
        updateRoom,
    } = useRooms();
    const { portals, addPortal, removePortal, setPortals, updatePortal } =
        usePortals();

    // ID of a portal to delete:
    const [deletingPortal, setDeletingPortal] = useState(null);
    // A parsed ref to either an existing portal,
    // or a room we want to create a new portal from:
    const [editingPortal, setEditingPortal] = useState(null);
    // Name of a property of the selected room we want to edit:
    const [editingProperty, setEditingProperty] = useState(null);
    // A map from room IDs to their fill colors:
    const [roomFills, setRoomFills] = useState(() => new Map());
    const [selectRoom, setSelectRoom] = useState(() => setSelectedRoomId);
    const [showLoader, setShowLoader] = useState(true);

    const viewRef = useRef({});

    const selectedRoom = rooms.get(selectedRoomId);

    useEffect(async () => {
        const rooms = await sendApiCall("entity-list room");
        console.log("Got rooms");
        setRooms(rooms);

        const portals = await sendApiCall("entity-list portal");
        console.log("Got portals");
        setPortals(portals);

        console.log("Done");

        setShowLoader(false);
    }, []);

    useEffect(() => {
        const onKeyDown = event => {
            const { tagName } = event.target;
            if (tagName === "INPUT" || tagName === "TEXTAREA") {
                return;
            }

            if (event.keyCode === Keys.LEFT_ARROW) {
                move(-1, 0);
            } else if (event.keyCode === Keys.UP_ARROW) {
                move(0, -1);
            } else if (event.keyCode === Keys.RIGHT_ARROW) {
                move(1, 0);
            } else if (event.keyCode === Keys.DOWN_ARROW) {
                move(0, 1);
            }
        };

        document.body.addEventListener("keydown", onKeyDown);
        return () => document.body.removeEventListener("keydown", onKeyDown);
    }, []);

    const addPortalToRoom = (roomRef, portalRef) => {
        const room = rooms.get(getId(roomRef));
        updateRoomProperty(room)("portals", [...room.portals, portalRef]);
    };

    const createPortal = async portal => {
        if (typeof portal.room2 !== "string") {
            const sourceRoom = rooms.get(getId(portal.room));
            const newRoom = {
                description: "",
                flags: sourceRoom.flags,
                name: "",
                portals: [],
                position: portal.room2.position,
            };
            const room2Ref = await sendApiCall(
                `entity-create room ${JSON.stringify(newRoom)}`
            );

            addRoom(newRoom);
            portal.room2 = room2Ref;
        }

        const portalRef = await sendApiCall(
            `entity-create portal ${JSON.stringify(portal)}`
        );

        addPortal(portal);
        addPortalToRoom(portal.room, portalRef);
        addPortalToRoom(portal.room2, portalRef);
    };

    const deletePortal = () => {
        const portal = portals.get(editingPortal[1]);
        const room = rooms.get(getId(portal.room));
        if (room) {
            updateRoomProperty(room)(
                "portals",
                room.portals.filter(portalRef => getId(portalRef) !== portal.id)
            );
        }
        const room2 = rooms.get(getId(portal.room2));
        if (room2) {
            updateRoomProperty(room2)(
                "portals",
                room2.portals.filter(
                    portalRef => getId(portalRef) !== portal.id
                )
            );
        }

        sendApiCall(`entity-delete ${deletingPortal}`);
        removePortal(portal.id);
        setDeletingPortal(null);
        setEditingPortal(null);
    };

    const deletePortalAccess = () => {
        updateRoomProperty(selectedRoom)(
            "portals",
            selectedRoom.portals.filter(
                portalRef => getId(portalRef) !== portal.id
            )
        );
        setDeletingPortal(null);
        setEditingPortal(null);
    };

    const enterRoom = () => {
        sendCommand(`enter-room #${selectedRoom.id}`);
        closeMapEditor();
    };

    const exportAsSvg = () => {
        const svg = viewRef.current.exportSvg();

        const el = document.createElement("a");
        el.setAttribute(
            "href",
            "data:image/svg+xml;utf8," + encodeURIComponent(svg)
        );
        el.setAttribute("download", "map.svg");

        el.style.display = "none";
        document.body.appendChild(el);

        el.click();

        document.body.removeChild(el);
    };

    const plotAltitude = () => {
        const data = {};
        for (const room of rooms.values()) {
            data[`room:${room.id}`] = room.position[2];
        }
        setRoomFills(parseData(rooms, data));
    };

    const plotStats = type => {
        sendApiCall(`log-retrieve stats ${type} 100`, data => {
            setRoomFills(parseData(rooms, data));
        });
    };

    const print = () => {
        const svg = viewRef.current.exportSvg();
        const target = window.open("", "_blank");
        target.document.write(svg);
        target.window.print();
    };

    const requestSelectRoom = () =>
        new Promise(resolve => setSelectRoom(() => resolve)).finally(() =>
            setSelectRoom(() => setSelectedRoomId)
        );

    const savePortal = async portal => {
        if (portal.id) {
            await sendApiCall(
                `entity-set portal:${portal.id} ${JSON.stringify(portal)}`
            );
            updatePortal(portal);
        } else {
            await createPortal(portal);
        }

        setEditingPortal(null);
    };

    const updateRoomProperty = room => (name, value) => {
        const updatedRoom = { ...room, [name]: value };
        updateRoom(updatedRoom);

        let serializedValue;
        if (Array.isArray(value)) {
            serializedValue = `[${value.join(",")}]`;
        } else {
            serializedValue = value;
        }

        sendApiCall(`property-set room:${room.id} ${name} ${serializedValue}`);
    };

    return html`
        <${Toolbar}
            exportAsSvg=${exportAsSvg}
            plotAltitude=${plotAltitude}
            plotPlayerDeaths=${() => plotStats("playerdeath")}
            plotRoomVisits=${() => plotStats("roomvisit")}
            print=${print}
        />
        <${Sidebar}
            onAddPortal=${() => setEditingPortal(["room", selectedRoomId])}
            onChange=${updateRoomProperty(selectedRoom)}
            onEditDescription=${() => setEditingProperty("description")}
            onEditPortal=${portalId => setEditingPortal(["portal", portalId])}
            onEnter=${enterRoom}
            selectedRoomId=${selectedRoomId}
        />
        <${MapView}
            imperativeHandle=${viewRef.current}
            onSelectRoom=${selectRoom}
            selectedRoomId=${selectedRoomId}
            roomFills=${roomFills}
        />
        <${NavigationPad} />
        ${deletingPortal
            ? html`<${DeletePortalDialog}
                  onClose=${() => setDeletingPortal(null)}
                  onDeleteAccess=${deletePortalAccess}
                  onDeletePortal=${deletePortal}
              />`
            : editingPortal &&
              html`<${PortalEditor}
                  onClose=${() => {
                      setEditingPortal(null);
                      setSelectRoom(() => setSelectedRoomId);
                  }}
                  onDelete=${() => setDeletingPortal(editingPortal[1])}
                  onSubmit=${savePortal}
                  portal=${editingPortal[0] === "room"
                      ? { ...newPortal, room: formatRef(editingPortal) }
                      : portals.get(editingPortal[1])}
                  requestSelectRoom=${requestSelectRoom}
              />`}
        ${editingProperty &&
        html`<${PropertyEditor}
            onClose=${() => setEditingProperty(null)}
            onSubmit=${value => {
                updateRoomProperty(selectedRoom)(editingProperty, value);
                setEditingProperty(null);
            }}
            value=${selectedRoom[editingProperty]}
        />`}
        ${showLoader && html`<${Loader} />`}
    `;
}

function parseData(rooms, data) {
    let lowest = 0;
    let highest = 0;
    for (const value of Object.values(data)) {
        if (value > highest) {
            highest = value;
        } else if (value < lowest) {
            lowest = value;
        }
    }

    const roomFills = new Map();
    for (const id of rooms.keys()) {
        const value = data[`room:${id}`] ?? 0;

        let red = 0;
        let green = 0;
        let blue = 0;
        if (lowest === 0) {
            if (highest === 0) {
                blue = 255;
            } else {
                red = Math.floor((255 * value) / highest);
                blue = 255 - red;
            }
        } else if (value > 0) {
            red = Math.floor((255 * value) / highest);
            green = 255 - red;
        } else if (value === 0) {
            green = 255;
        } else {
            blue = Math.floor((255 * value) / lowest);
            green = 255 - blue;
        }

        roomFills.set(id, `rgb(${red}, ${green}, ${blue})`);
    }

    return roomFills;
}
