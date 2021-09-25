import {
    addCommandListener,
    sendApiCall,
    sendCommand,
    setFocus,
    writeToScreen,
} from "./main.js";
import { openMapEditor } from "./map_editor/mod.js";

const statusHeader = document.querySelector(".status-header");
statusHeader.innerHTML += ' <a class="edit-map">Edit Map</a>';

const editMapLink = statusHeader.querySelector(".edit-map");
editMapLink.addEventListener("click", openMapEditor);

document.querySelector(".command-input").removeAttribute("maxlength");

function attachListeners() {
    addCommandListener(command => {
        const [commandName] = command.split(" ");
        const rest = command.slice(commandName.length + 1);

        if (
            commandName.startsWith("edit-p") &&
            "edit-property".startsWith(commandName)
        ) {
            sendApiCall("property-get " + rest).then(data => {
                const { propertyName } = data;
                if (data.readOnly) {
                    writeToScreen(`Property ${propertyName} is read-only.`);
                    return;
                }

                propertyEditor.edit(data[propertyName], {
                    onsave: value => {
                        sendCommand(
                            `set-prop #${
                                data.id
                            } ${propertyName} ${value.replace(/\n/g, "\\n")}`
                        );
                        propertyEditor.close();
                    },
                    onclose: setFocus,
                });
            });

            return false;
        }

        if (command === "exec-script") {
            propertyEditor.edit("", {
                mode: "javascript",
                onsave: value => {
                    sendCommand(`exec-script ${value.replace(/\s+/g, " ")}`);
                    propertyEditor.close();
                },
                onclose: setFocus,
                submitButtonLabel: "Execute",
            });

            return false;
        }

        if (command.startsWith("@")) {
            if (command.includes(" ")) {
                return "set-prop room " + command.slice(1);
            } else {
                return "get-prop room " + command.slice(1);
            }
        }

        if (command.startsWith("api-")) {
            sendApiCall(command.slice(4)).then(data => {
                writeToScreen(JSON.stringify(data, null, 2));
            });

            return false;
        }
    });
}

attachListeners();
