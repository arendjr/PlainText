/*global define:false, require:false*/
define(["controller", "mapeditor/mapeditor", "propertyeditor/propertyeditor", "lib/zepto"],
       function(Controller, MapEditor, PropertyEditor, $) {

    "use strict";

    var mapEditor;

    var propertyEditor;

    var triggers = {};


    function init() {

        mapEditor = new MapEditor();

        propertyEditor = new PropertyEditor();

        var editMapLink = $("<a />", {
            "text": "Edit Map",
            "href": ["javas", "cript:void(0)"].join("")
        });
        editMapLink.on("click", function() {
            mapEditor.open();
        });

        var statusHeader = $(".status-header");
        statusHeader.append(" ");
        statusHeader.append(editMapLink);

        $(".command-input").removeAttr("maxlength");

        attachListeners();

        fetchTriggers();
    }

    function attachListeners() {

        Controller.addCommandListener(function(command) {
            var commandName = command.split(" ")[0];
            var rest = command.substr(commandName.length + 1);

            if (commandName.startsWith("edit-p") && "edit-property".startsWith(commandName)) {
                Controller.sendApiCall("property-get " + rest, function(data) {
                    var propertyName = data.propertyName;
                    if (data.readOnly) {
                        Controller.writeToScreen("Property " + propertyName + " is read-only.");
                        return;
                    }

                    propertyEditor.edit(data[propertyName], {
                        "onsave": function(value) {
                            Controller.sendCommand("set-prop #" + data.id + " " + propertyName +
                                                   " " + value.replace(/\n/g, "\\n"));
                            propertyEditor.close();
                        },
                        "onclose": function() {
                            Controller.setFocus();
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
                    Controller.sendApiCall("trigger-get " + rest, function(data) {
                        var triggerSource = data.triggerSource;
                        if (!triggerSource) {
                            var trigger = triggers[triggerName];
                            if (trigger.includes("(")) {
                                var argsString = trigger.substring(trigger.indexOf("(") + 1,
                                                                   trigger.indexOf(")"));
                                var args = [];
                                argsString.split(", ").forEach(function(arg) {
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
                                Controller.sendApiCall("trigger-set " + data.id + " " +
                                                       triggerName + " " + value, function() {
                                    propertyEditor.close();
                                });
                            },
                            "onclose": function() {
                                Controller.setFocus();
                            }
                        });
                    });
                } else {
                    Controller.writeToScreen("There is no trigger named " + triggerName + ".");
                }

                return false;
            }

            if (command === "exec-script") {
                propertyEditor.edit("", {
                    "mode": "javascript",
                    "onsave": function(value) {
                        Controller.sendCommand("exec-script " + value.replace(/\s+/g, " "));
                        propertyEditor.close();
                    },
                    "onclose": function() {
                        Controller.setFocus();
                    },
                    "submitButtonLabel": "Execute"
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
                Controller.sendApiCall(command.substr(4), function(data) {
                    Controller.writeToScreen(JSON.stringify(data, null, 2));
                });

                return false;
            }
        });
    }

    function fetchTriggers() {

        Controller.sendApiCall("triggers-list", function(data) {
            data.forEach(function(trigger) {
                var triggerName;
                if (trigger.includes("(")) {
                    triggerName = trigger.substr(0, trigger.indexOf("("));
                } else {
                    triggerName = trigger.substr(0, trigger.indexOf(" : "));
                }
                triggers[triggerName] = trigger;
            });
        });
    }

    init();
});
