(function() {

    var self = controller;

    var propertyEditor = element(".property-editor");
    self.propertyEditor = propertyEditor;

    var saveCommand;
    var saveCallback;

    function loadPropertyEditor() {
        loadScript("codemirror/codemirror.js");
        loadStyle("codemirror/codemirror.css");

        propertyEditor.show = function(value) {
            if (value && value.trimmed().startsWith("(function")) {
                this.editor.setOption("mode", "javascript");
                this.editor.setOption("lineWrapping", false);
            } else {
                this.editor.setOption("mode", "null");
                this.editor.setOption("lineWrapping", true);
            }

            this.style.display = "block";
            this.editor.setValue(value !== undefined ? value.trimmed() : "");
            this.editor.focus();
        };
        propertyEditor.hide = function() {
            this.style.display = "none";
            self.commandInput.focus();
        };
        propertyEditor.edit = function(description, onSaved) {
            self.sendApiCall("property-get " + description, function(data) {
                var propertyName = data.propertyName;
                if (data.readOnly) {
                    self.writeToScreen("Property " + propertyName + " is read-only.");
                    return;
                }

                propertyEditor.show(data[propertyName]);

                saveCommand = "set-prop #" + data.id + " " + propertyName;
                saveCallback = onSaved;
            });
        };

        propertyEditor.querySelector(".cancel-button").addEventListener("click", function() {
            propertyEditor.hide();
        }, false);
        propertyEditor.querySelector(".save-button").addEventListener("click", function() {
            var value = propertyEditor.editor.getValue();
            var originalValue = value;
            if (saveCommand.startsWith("set-prop")) {
                value = value.replace(/\n/g, "\\n");
            } else if (saveCommand.startsWith("exec-script")) {
                value = value.replace(/\s+/g, " ");
            }

            if (saveCommand.startsWith("api-")) {
                self.sendApiCall(saveCommand.substr(4) + " " + value, function(data) {
                    self.writeToScreen(data);

                    propertyEditor.hide();
                    if (saveCallback) {
                        saveCallback(originalValue);
                        saveCallback = undefined;
                    }
                });
            } else {
                self.socket.send(saveCommand + " " + value);

                propertyEditor.hide();
                if (saveCallback) {
                    saveCallback(originalValue);
                    saveCallback = undefined;
                }
            }
        }, false);

        function initCodeMirror() {
            if (!window.CodeMirror) {
                setTimeout(initCodeMirror, 50);
                return;
            }

            loadScript("codemirror/javascript.js");
            loadScript("codemirror/util/simple-hint.js");
            loadScript("codemirror/util/javascript-hint.js");
            loadStyle("codemirror/util/simple-hint.css");

            propertyEditor.editor = CodeMirror.fromTextArea(element("#edit-field"), {
                lineNumbers: true,
                matchBrackets: true,
                tabSize: 4,
                indentUnit: 4,
                indentWithTabs: false
            });
        }

        setTimeout(initCodeMirror, 50);
    }

    function loadMapEditor() {
        loadScript("mapeditor.js");
    }

    loadPropertyEditor();
    loadMapEditor();

    var triggers = {};
    self.sendApiCall("triggers-list", function(data) {
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

    var onkeypress = controller.commandInput.onkeypress;
    function substituteCommand(event, commandToExecute, commandToPushToHistory) {
        self.commandInput.value = commandToExecute;
        if (commandToExecute.isEmpty()) {
            self.history.push(commandToPushToHistory);
        } else {
            onkeypress(event);
            self.history[self.history.length - 1] = commandToPushToHistory;
        }
    }

    controller.commandInput.onkeypress = function(event) {
        if (event.keyCode === keys.KEY_RETURN) {
            var command = self.commandInput.value;
            var commandName = command.split(" ")[0];
            var rest = command.substr(commandName.length + 1);

            if (commandName.startsWith("edit-p") && "edit-property".startsWith(commandName)) {
                substituteCommand(event, "", command);

                propertyEditor.edit(rest);
            } else if (commandName.startsWith("edit-t") && "edit-trigger".startsWith(commandName)) {
                substituteCommand(event, "", command);

                var triggerName = command.split(/\s+/)[2];
                if (parseInt(triggerName, 10) > 0) {
                    triggerName = command.split(/\s+/)[3];
                }

                if (triggers.hasOwnProperty(triggerName)) {
                    self.sendApiCall("trigger-get " + rest, function(data) {
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

                        propertyEditor.show(triggerSource);

                        saveCommand = "api-trigger-set #" + data.id + " " + triggerName;
                    });
                } else {
                    self.writeToScreen("There is no trigger named " + triggerName + ".");
                }
            } else if (command === "exec-script") {
                substituteCommand(event, "", command);

                propertyEditor.show();

                saveCommand = "exec-script";
            } else if (command.startsWith("@")) {
                if (command.contains(" ")) {
                    substituteCommand(event, "set-prop area " + command.substr(1), command);
                } else {
                    substituteCommand(event, "get-prop area " + command.substr(1), command);
                }
            } else if (command.startsWith("api-")) {
                self.sendApiCall(command.substr(4), function(data) {
                    self.writeToScreen(JSON.stringify(data, null, 2));
                });

                substituteCommand(event, "", command);
            } else {
                onkeypress(event);
            }
        } else {
            onkeypress(event);
        }
    };

    self.commandInput.removeAttribute("maxlength");
})();
