import { Keys, addStyle, sendApiCall, sendCommand, setFocus } from "../main.js";
import { MapEditor } from "./components/map_editor.js";
import { html, render } from "./lib.js";

addStyle("map_editor/mod.css");

let root = null;

export function openMapEditor() {
    root = document.createElement("div");
    root.setAttribute("class", "map-editor");
    document.body.appendChild(root);

    render(html`<${MapEditor} />`, root);
}

export function closeMapEditor() {
    root.remove();
    root = null;

    setFocus();
}

/*export default class MapEditor {
    constructor() {
        const initialZoom = 0.2;

        this.model = new Laces.Model();
        this.model.set("map", new MapModel());
        this.model.set("selectedRoom", null);

        const tie = new Laces.Tie(
            this.model,
            `<div class="map-editor" style="display: none">
                <div class="sidebar">
                    <div class="selected-room" data-laces-visible="selectedRoom">
                        <div class="help" title="Double-click any property to edit"></div>
                        <h2 data-laces="{ property: selectedRoom.name, editable: true, default: Unnamed Room }"></h2>
                        <p>
                            <span class="label">Room #<span data-laces-property="selectedRoom.id"></span></span>
                            <button class="enter-room-button"
                                title="Enter this room with your in-game character, regardless of where you currently are.">Enter</button>
                        </p>
                        <p>
                            <input type="number" class="x" data-laces-property="selectedRoom.x">,
                            <input type="number" class="y" data-laces-property="selectedRoom.y">,
                            <input type="number" class="z" data-laces-property="selectedRoom.z">
                        </p>
                        <p>
                            <span class="label">Area</span>
                            <span class="area" data-laces-property="selectedRoom.areaName"></span>
                        </p>
                        <p>
                            <span class="label">Description</span>
                            <a href="javascript:void(0)" data-laces-visible="!selectedRoom.description"
                                class="add description"></a><br>
                            <span class="description" data-laces-property="selectedRoom.description"></span>
                        </p>
                        <p>
                            <span class="label">Portals</span>
                            <span class="portals"></span>
                            <a href="javascript:void(0)" class="add portal"></a>
                        </p>
                        <p>
                            <span class="label">Options</span>
                        </p>
                        <p class="option">
                            <span title="If checked, dynamic room descriptions include information about portals.">
                                <input id="omitDynamicPortals" type="checkbox"
                                    data-laces-property="selectedRoom.dynamicPortalDescriptions">
                                <label for="omitDynamicPortals">Dynamic portals descriptions</label>
                            </span>
                        </p>
                        <p class="option">
                            <span
                                title="If checked, dynamic room descriptions include information about characters in the distance.">
                                <input id="omitDistantCharacters" type="checkbox"
                                    data-laces-property="selectedRoom.distantCharacterDescriptions">
                                <label for="omitDistantCharacters">Distant character descriptions</label>
                            </span>
                        </p>
                        <p class="option">
                            <span title="Whether the room has walls or not is important for calculating line-of-sight.">
                                <input id="hasWalls" type="checkbox" data-laces-property="selectedRoom.hasWalls">
                                <label for="hasWalls">Has walls</label>
                            </span>
                        </p>
                        <p class="option">
                            <span title="Whether the room has a ceiling or not is important for calculating line-of-sight.">
                                <input id="hasCeiling" type="checkbox" data-laces-property="selectedRoom.hasCeiling">
                                <label for="hasCeiling">Has ceiling</label>
                            </span>
                        </p>
                        <p class="option">
                            <span title="Whether the room has a floor or not is important for calculating line-of-sight.">
                                <input id="hasFloor" type="checkbox" data-laces-property="selectedRoom.hasFloor">
                                <label for="hasFloor">Has floor</label>
                            </span>
                        </p>
                        <p class="option">
                            <span title="Does this room represent (part of) a road?">
                                <input id="isRoad" type="checkbox" data-laces-property="selectedRoom.isRoad">
                                <label for="isRoad">Is road</label>
                            </span>
                        </p>
                        <p class="option">
                            <span title="Does this room represent (part of) a river?">
                                <input id="isRiver" type="checkbox" data-laces-property="selectedRoom.isRiver">
                                <label for="isRiver">Is river</label>
                            </span>
                        </p>
                        <p class="option">
                            <span title="Does this room represent (part of) a roof?">
                                <input id="isRoof" type="checkbox" data-laces-property="selectedRoom.isRoof">
                                <label for="isRoof">Is roof</label>
                            </span>
                        </p>
                    </div>
                </div>
            </div>`
        );
        document.body.appendChild(tie.render());
        this.element = $(".map-editor");

        this.portalEditor = new PortalEditor();
        this.portalEditor.setMapModel(this.model.map);
        this.portalEditor.setMapView(this.view);

        this.portalDeleteDialog = new PortalDeleteDialog();

        this.propertyEditor = new PropertyEditor();

        this.attachListeners();
    }

    attachListeners() {
        this.model.bind("change:selectedRoom", () => {
            const room = this.model.selectedRoom;
            if (room) {
                const portalsSpan = $(".portals", this.element);
                portalsSpan.empty();
                for (const portal of room.portals.filter(Boolean)) {
                    if (portalsSpan.children().length) {
                        portalsSpan.append(", ");
                    }

                    const portalSpan = $("<a />", {
                        text: portal.nameFromRoom(room),
                        class: "edit portal",
                        "data-portal-id": portal.id,
                        href: ["java", "script:void(0)"].join(""),
                    });
                    portalsSpan.append(portalSpan);
                }
            }
        });

        this.view.addSelectionListener(selectedRoomId => {
            this.model.selectedRoom = this.model.map.rooms[selectedRoomId];
        });

        $(".export-as-svg", this.element).on("click", () => {
            const target = window.open("", "_blank");
            this.view.exportSvg().then(data => {
                target.document.write(data);
            });
        });

        $(".print", this.element).on("click", () => {
            this.view.print();
        });

        $(".plot.altitude", this.element).on("click", () => {
            this.plotAltitude();
        });

        $(".plot.roomvisit", this.element).on("click", () => {
            this.plotStats("roomvisit");
        });

        $(".plot.playerdeath", this.element).on("click", () => {
            this.plotStats("playerdeath");
        });

        $(".enter-room-button", this.element).on("click", () => {
            if (this.model.selectedRoom) {
                sendCommand("enter-room #" + this.model.selectedRoom.id);
                this.close();
            }
        });

        const editDescription = () => {
            const room = this.model.selectedRoom;
            if (room) {
                this.propertyEditor.edit(room.description, {
                    onsave: description => {
                        room.description = description;
                        this.propertyEditor.close();
                    },
                });
            }
        };
        $(".description", this.element).on("dblclick", editDescription);
        $(".add.description", this.element).on("click", editDescription);

        this.element.on("click", ".edit.portal", event => {
            const portal =
                this.model.map.portals[
                    event.target.getAttribute("data-portal-id")
                ];
            this.portalEditor.edit(portal, {
                ondelete: portalId => {
                    if (
                        portal.room.portals.includes(portal) &&
                        portal.room2.portals.includes(portal)
                    ) {
                        this.portalDeleteDialog.show({
                            ondeleteone: () => {
                                const sourceRoom =
                                    this.model.map.rooms[this.selectedRoom.id];
                                sourceRoom.portals = sourceRoom.portals.filter(
                                    p => p !== portal
                                );
                                this.portalDeleteDialog.close();
                                this.portalEditor.close();
                            },
                            ondeleteboth: () => {
                                this.model.map.portals.remove(portalId);
                                this.portalDeleteDialog.close();
                                this.portalEditor.close();
                            },
                        });
                    } else {
                        this.model.map.portals.remove(portalId);
                    }
                },
            });
        });

        $(".add.portal", this.element).on("click", () =>
            this.portalEditor.add(this.model.selectedRoom)
        );

        $(document.body).on("keydown", event => {
            if (
                event.target.tagName === "INPUT" ||
                event.target.tagName === "TEXTAREA"
            ) {
                return;
            }

            if (event.keyCode === Keys.LEFT_ARROW) {
                this.view.move(-1, 0);
            } else if (event.keyCode === Keys.UP_ARROW) {
                this.view.move(0, -1);
            } else if (event.keyCode === Keys.RIGHT_ARROW) {
                this.view.move(1, 0);
            } else if (event.keyCode === Keys.DOWN_ARROW) {
                this.view.move(0, 1);
            }
        });
    }

    plotStats(type) {
        sendApiCall(`log-retrieve stats ${type} 100`).then(data => {
            for (let key in data) {
                const id = key.slsubstrsubstrsubstrice(5);
                data[id] = data[key];
                delete data[key];
            }

            this.view.plotData(data);
        });
    }

    plotAltitude() {
        const { rooms } = this.model.map;
        const data = {};
        for (var id in rooms) {
            data[id] = rooms[id].z;
        }
        this.view.plotData(data);
    }
}*/
