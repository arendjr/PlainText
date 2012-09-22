(function() {

    var self = controller;

    var propertyEditor = element(".property-editor");

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

        element("#edit-cancel-button").onclick = function() {
            propertyEditor.hide();
        };
        element("#edit-submit-button").onclick = function() {
            var value = propertyEditor.editor.getValue();
            if (saveCommand.startsWith("set-prop")) {
                value = value.replace(/\n/g, "\\n");
            } else if (saveCommand.startsWith("exec-script")) {
                value = value.replace(/\s+/g, " ");
            }

            if (saveCommand.startsWith("api-")) {
                self.sendApiCall("save1", saveCommand.substr(4) + " " + value, function(data) {
                    self.writeToScreen(data);

                    propertyEditor.hide();
                });
            } else {
                self.socket.send(saveCommand + " " + value);

                propertyEditor.hide();
            }
        };

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
    self.sendApiCall("triggers1", "triggers-list", function(data) {
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

    var saveCommand;

    controller.commandInput.onkeypress = function(event) {
        if (event.keyCode === keys.KEY_RETURN) {
            var command = self.commandInput.value;
            var commandName = command.split(" ")[0];
            var rest = command.substr(commandName.length);

            if (commandName === "edit-prop" || commandName === "edit-p") {
                substituteCommand(event, "", command);

                self.sendApiCall("prop1", "property-get" + rest, function(data) {
                    var propertyName = data.propertyName;
                    if (data.readOnly) {
                        self.writeToScreen("Property " + propertyName + " is read-only.");
                        return;
                    }

                    propertyEditor.show(data[propertyName]);

                    saveCommand = "set-prop #" + data.id + " " + propertyName;
                });
            } else if (commandName === "edit-trigger" || commandName === "edit-t") {
                substituteCommand(event, "", command);

                var triggerName = command.split(/\s+/)[2];
                if (parseInt(triggerName, 10) > 0) {
                    triggerName = command.split(/\s+/)[3];
                }

                if (triggers.hasOwnProperty(triggerName)) {
                    self.sendApiCall("trigger1", "trigger-get" + rest, function(data) {
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
            } else {
                onkeypress(event);
            }
        } else {
            onkeypress(event);
        }
    };

    self.commandInput.removeAttribute("maxlength");
})();
