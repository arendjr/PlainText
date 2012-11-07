(function() {

    var mapEditor = null;

    var propertyEditor = null;

    var triggers = {};

    function init() {

        loadScript("admin/__all__.js", function() {
            mapEditor = new MapEditor(element(".map-editor"));

            var editMapLink = document.createElement("a");
            editMapLink.setAttribute("href", "javascript:void(0)");
            editMapLink.textContent = "Edit Map";
            editMapLink.addEventListener("click", function() {
                mapEditor.open();
            }, false);

            var statusHeader = element(".status-header");
            statusHeader.appendChild(document.createTextNode(" "));
            statusHeader.appendChild(editMapLink);

            propertyEditor = new PropertyEditor();
            controller.propertyEditor = propertyEditor;
        });

        loadStyle("admin/admin.css");

        elements("a[href='#']").forEach(function(element) {
            element.setAttribute("href", "javascript:void(0)");
        });

        controller.commandInput.removeAttribute("maxlength");

        attachListeners();

        fetchTriggers();
    }

    function attachListeners() {

        controller.addCommandListener(function(command) {
            var commandName = command.split(" ")[0];
            var rest = command.substr(commandName.length + 1);

            if (commandName.startsWith("edit-p") && "edit-property".startsWith(commandName)) {
                controller.sendApiCall("property-get " + rest, function(data) {
                    var propertyName = data.propertyName;
                    if (data.readOnly) {
                        controller.writeToScreen("Property " + propertyName + " is read-only.");
                        return;
                    }

                    propertyEditor.edit(data[propertyName], {
                        "onsave": function(value) {
                            controller.socket.send("set-prop #" + data.id + " " + propertyName +
                                                   " " + value.replace(/\n/g, "\\n"));
                            propertyEditor.close();
                        },
                        "onclose": function() {
                            controller.setFocus();
                        }
                    });
                });

                return false;
            }

            if (commandName.startsWith("edit-t") && "edit-trigger".startsWith(commandName)) {
                var triggerName = command.split(/\s+/)[2];
                if (parseInt(triggerName, 10) > 0) {
                    triggerName = command.split(/\s+/)[3];
                }

                if (triggers.hasOwnProperty(triggerName)) {
                    controller.sendApiCall("trigger-get " + rest, function(data) {
                        var triggerSource = data.triggerSource;
                        if (triggerSource.isEmpty()) {
                            var trigger = triggers[triggerName];
                            if (trigger.contains("(")) {
                                var arguments = trigger.substring(trigger.indexOf("(") + 1,
                                                                  trigger.indexOf(")"));
                                var args = [];
                                arguments.split(", ").forEach(function(arg) {
                                    args.push(arg.substr(0, arg.indexOf(" : ")));
                                });
                                triggerSource = "(function(" + args.join(", ") + ") {\n    \n})";
                            } else {
                                triggerSource = "(function() {\n    \n})";
                            }
                        }

                        propertyEditor.edit(triggerSource, {
                            "mode": "javascript",
                            "onsave": function(value) {
                                controller.sendApiCall("trigger-set #" + data.id + " " +
                                                       triggerName + " " + value, function() {
                                    propertyEditor.close();
                                });
                            },
                            "onclose": function() {
                                controller.setFocus();
                            }
                        });
                    });
                } else {
                    controller.writeToScreen("There is no trigger named " + triggerName + ".");
                }

                return false;
            }

            if (command === "exec-script") {
                propertyEditor.edit("", {
                    "mode": "javascript",
                    "onsave": function(value) {
                        controller.socket.send("exec-script " + value.replace(/\s+/g, " "));
                        propertyEditor.close();
                    },
                    "onclose": function() {
                        controller.setFocus();
                    },
                    "submitButtonLabel": "Execute"
                });

                return false;
            }

            if (command.startsWith("@")) {
                if (command.contains(" ")) {
                    return "set-prop room " + command.substr(1);
                } else {
                    return "get-prop room " + command.substr(1);
                }
            }

            if (command.startsWith("api-")) {
                controller.sendApiCall(command.substr(4), function(data) {
                    controller.writeToScreen(JSON.stringify(data, null, 2));
                });

                return false;
            }
        });
    }

    function fetchTriggers() {

        controller.sendApiCall("triggers-list", function(data) {
            for (var i = 0; i < data.length; i++) {
                var trigger = data[i];
                var triggerName;
                if (trigger.contains("(")) {
                    triggerName = trigger.substr(0, trigger.indexOf("("));
                } else {
                    triggerName = trigger.substr(0, trigger.indexOf(" : "));
                }
                triggers[triggerName] = trigger;
            }
        });
    }

    init();

})();

scriptLoaded("admin.js");
