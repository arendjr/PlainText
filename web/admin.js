import MapEditor from "./mapeditor/mapeditor.js";
import PropertyEditor from "./propertyeditor/propertyeditor.js";
import {
    addCommandListener,
    sendApiCall,
    sendCommand,
    setFocus,
    writeToScreen,
} from "./main.js";

const triggers = new Map();

const mapEditor = new MapEditor();

const propertyEditor = new PropertyEditor();

const editMapLink = $("<a />", {
    text: "Edit Map",
    href: ["javas", "cript:void(0)"].join(""),
});
editMapLink.on("click", function () {
    mapEditor.open();
});

const statusHeader = $(".status-header");
statusHeader.append(" ");
statusHeader.append(editMapLink);

$(".command-input").removeAttr("maxlength");

function attachListeners() {
    addCommandListener(command => {
        const [commandName] = command.split(" ");
        const rest = command.substr(commandName.length + 1);

        if (
            commandName.startsWith("edit-p") &&
            "edit-property".startsWith(commandName)
        ) {
            sendApiCall("property-get " + rest, data => {
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

        if (
            commandName.startsWith("edit-t") &&
            "edit-trigger".startsWith(commandName)
        ) {
            const triggerName = command.split(/\s+/)[2];
            if (parseInt(triggerName, 10) > 0) {
                triggerName = command.split(/\s+/)[3];
            }

            const trigger = triggers.get(triggerName);
            if (trigger) {
                sendApiCall("trigger-get " + rest, data => {
                    const { triggerSource } = data;
                    if (!triggerSource) {
                        if (trigger.includes("(")) {
                            const argsString = trigger.substring(
                                trigger.indexOf("(") + 1,
                                trigger.indexOf(")")
                            );
                            const args = argsString
                                .split(", ")
                                .map(arg => arg.substr(0, arg.indexOf(" : ")));
                            triggerSource = `(function(${args.join(
                                ", "
                            )}) {\n    \n})`;
                        } else {
                            triggerSource = "(function() {\n    \n})";
                        }
                    }

                    propertyEditor.edit(triggerSource, {
                        mode: "javascript",
                        onsave: value => {
                            sendApiCall(
                                `trigger-set ${data.id} ${triggerName} ${value}`,
                                () => propertyEditor.close()
                            );
                        },
                        onclose: setFocus,
                    });
                });
            } else {
                writeToScreen(`There is no trigger named ${triggerName}.`);
            }

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
                return "set-prop room " + command.substr(1);
            } else {
                return "get-prop room " + command.substr(1);
            }
        }

        if (command.startsWith("api-")) {
            sendApiCall(command.substr(4), data => {
                writeToScreen(JSON.stringify(data, null, 2));
            });

            return false;
        }
    });
}

function fetchTriggers() {
    sendApiCall("triggers-list", data => {
        for (const trigger of data) {
            const triggerName = trigger.substr(
                0,
                trigger.includes("(")
                    ? trigger.indexOf("(")
                    : trigger.indexOf(" : ")
            );
            triggers.set(triggerName, trigger);
        }
    });
}

attachListeners();

fetchTriggers();
