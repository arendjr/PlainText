import { Keys } from "../util.js";
import Laces from "../lib/laces.tie.js";
import MapModel from "../mapmodel/model.js";
import MapView, { SVG_URI } from "./mapview.js";
import PortalEditor from "../portaleditor/portaleditor.js";
import PortalDeleteDialog from "../portaleditor/portaldeletedialog.js";
import PropertyEditor from "../propertyeditor/propertyeditor.js";
import SliderWidget from "../sliderwidget/slider.js";
import { addStyle, sendApiCall, sendCommand, setFocus } from "../main.js";
import { showLoader } from "../loadingwidget/loading.js";

export default class MapEditor {
    constructor() {
        const initialZoom = 0.2;

        this.model = new Laces.Model();
        this.model.set("map", new MapModel());
        this.model.set("selectedRoom", null);

        addStyle("mapeditor/mapeditor");

        const tie = new Laces.Tie(
            this.model,
            `<div class="map-editor" style="display: none">
                <svg class="map-canvas" viewBox="0 0 100 100" xmlns="${SVG_URI}">
                    <line x1="116" y1="1026" x2="116" y2="746" stroke="green" stroke-width="2" opacity="0.4"></line>
                </svg>
                <div class="toolbar">
                    <div class="label menu">Map
                        <div class="menu-content">
                            <div class="menu-item">
                                <a href="javascript:void(0)" class="export-as-svg">Export as SVG</a>
                            </div>
                            <div class="menu-item">
                                <a href="javascript:void(0)" class="print">Print...</a>
                            </div>
                        </div>
                    </div>
                    <div class="label menu">Options
                        <div class="menu-content">
                            <div class="menu-item">
                                <a href="javascript:void(0)" class="plot altitude">Plot altitude</a>
                            </div>
                            <div class="menu-item">
                                <a href="javascript:void(0)" class="plot roomvisit">Plot room visits</a>
                            </div>
                            <div class="menu-item">
                                <a href="javascript:void(0)" class="plot playerdeath">Plot player deaths</a>
                            </div>
                            <div class="menu-item">
                                <input id="toggle-room-names" type="checkbox">
                                <label for="toggle-room-names"> Show room names</label>
                            </div>
                            <div class="menu-item">
                                <input id="toggle-z-restriction" type="checkbox">
                                <label for="toggle-z-restriction"> Restrict Z-level to: </label>
                                <input class="z-restriction" type="number" value="0">
                            </div>
                        </div>
                    </div>
                    <div class="label">Zoom: </div>
                    <div class="zoom slider"></div>
                    <div class="label">Perspective: </div>
                    <div class="perspective slider"></div>
                    <div class="close"></div>
                </div>
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
                <div class="pad">
                    <div class="up arrow"></div>
                    <div class="right arrow"></div>
                    <div class="down arrow"></div>
                    <div class="left arrow"></div>
                </div>
            </div>`
        );
        document.body.appendChild(tie.render());
        this.element = $(".map-editor");

        this.view = new MapView(this.element[0].querySelector(".map-canvas"));
        this.view.setModel(this.model.map);
        this.view.setZoom(initialZoom);

        this.portalEditor = new PortalEditor();
        this.portalEditor.setMapModel(this.model.map);
        this.portalEditor.setMapView(this.view);

        this.portalDeleteDialog = new PortalDeleteDialog();

        this.propertyEditor = new PropertyEditor();

        this.zoomSlider = new SliderWidget($(".zoom.slider", this.element)[0], {
            width: 200,
            initialValue: initialZoom,
        });
        this.perspectiveSlider = new SliderWidget(
            $(".perspective.slider", this.element)[0],
            { width: 300 }
        );

        this.attachListeners();
    }

    attachListeners() {
        this.model.bind("change:selectedRoom", () => {
            const room = this.model.selectedRoom;
            if (room) {
                const portalsSpan = $(".portals", this.element);
                portalsSpan.empty();
                room.portals.forEach(portal => {
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
                });
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

        $("#toggle-room-names", this.element).on("change", () => {
            const isChecked = $("#toggle-room-names", this.element).prop(
                "checked"
            );
            this.view.setDisplayRoomNames(isChecked);
        });

        $("#toggle-z-restriction,.z-restriction", this.element).on(
            "change",
            () => {
                const isChecked = $("#toggle-z-restriction", this.element).prop(
                    "checked"
                );
                this.view.setZRestriction(
                    isChecked ? $(".z-restriction").val() : null
                );
            }
        );

        $(".close", this.element).on("click", () => this.close());

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

        $(".up.arrow", this.element).on("click", () => {
            this.view.move(0, -1);
        });
        $(".right.arrow", this.element).on("click", () => {
            this.view.move(1, 0);
        });
        $(".down.arrow", this.element).on("click", () => {
            this.view.move(0, 1);
        });
        $(".left.arrow", this.element).on("click", () => {
            this.view.move(-1, 0);
        });

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

        this.zoomSlider.element.addEventListener("change", event =>
            this.view.setZoom(event.detail.value)
        );

        this.perspectiveSlider.element.addEventListener("change", event =>
            this.view.setPerspective(event.detail.value)
        );
    }

    open() {
        showLoader();

        this.element.show();

        this.model.map.load();
    }

    close() {
        this.element.hide();

        setFocus();
    }

    plotStats(type) {
        sendApiCall("log-retrieve stats " + type + " 100", data => {
            for (let key in data) {
                const id = key.substr(5);
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
}
