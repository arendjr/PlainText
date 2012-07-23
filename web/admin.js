(function() {

    var self = controller;

    var triggers = {};
    self.onMessageHook = function(message) {
        message = message.trimmed();

        var lines = message.split("\n");
        lines.forEach(function(line) {
            if (line.startsWith("  \x1B[37;1m")) {
                var trigger = line.substring(9, line.indexOf("\x1B[0m"));
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
    self.socket.send("help triggers");

    var onkeypress = controller.commandInput.onkeypress;

    controller.commandInput.onkeypress = function(event) {
        if (event.keyCode === keys.KEY_RETURN) {
            var overwriteHistory = false;

            var command = self.commandInput.value;
            var commandName = command.split(" ")[0];
            if (commandName === "edit-prop") {
                overwriteHistory = true;

                self.setCommand = "set" + command.substr(4);
                self.commandInput.value = "get" + command.substr(4);

                self.editScreen.style.display = "block";

                self.onMessageHook = function(message) {
                    message = message.trimmed();

                    self.editField.value = message;
                    self.editField.focus();
                }
            } else if (commandName === "edit-trigger") {
                overwriteHistory = true;

                var triggerName = command.split(/\s+/)[2];
                if (parseInt(triggerName, 10) > 0) {
                    triggerName = command.split(/\s+/)[3];
                }

                if (triggers.hasOwnProperty(triggerName)) {
                    self.setCommand = "set" + command.substr(4);
                    self.commandInput.value = "get" + command.substr(4);

                    self.onMessageHook = function(message) {
                        message = message.trimmed();

                        if (message.startsWith("No trigger set for")) {
                            var trigger = triggers[triggerName];
                            if (trigger.contains("(")) {
                                var arguments = trigger.substring(trigger.indexOf("(") + 1,
                                                                  trigger.indexOf(")"));
                                var args = [];
                                arguments.split(", ").forEach(function(arg) {
                                    args.push(arg.substr(0, arg.indexOf(" : ")));
                                });
                                message = "(function(" + args.join(", ") + ") {\n    \n})";
                            } else {
                                message = "(function() {\n    \n})";
                            }
                        }

                        if (message === "Object not found.") {
                            self.writeToScreen(message);
                        } else {
                            self.editScreen.style.display = "block";

                            self.editField.value = message;

                            self.editField.selectionStart = message.length - 3;
                            self.editField.selectionEnd = self.editField.selectionStart;

                            self.editField.focus();
                        }
                    }
                } else {
                    self.writeToScreen("There is no trigger named " + triggerName + ".");
                    self.commandInput.value = "";
                }
            } else if (command === "exec-script") {
                overwriteHistory = true;

                self.setCommand = "exec-script";
                self.commandInput.value = "";

                self.editScreen.style.display = "block";
                self.editField.focus();
            } else if (command.startsWith("@")) {
                overwriteHistory = true;

                if (command.contains(" ")) {
                    self.commandInput.value = "set-prop area " + command.substr(1);
                } else {
                    self.commandInput.value = "get-prop area " + command.substr(1);
                }
            }

            onkeypress(event);

            if (overwriteHistory) {
                self.history[self.history.length - 1] = command;
            }
        } else {
            onkeypress(event);
        }
    }

    controller.commandInput.removeAttribute("maxlength");

    document.getElementById("edit-cancel-button").onclick = function() {
        self.editScreen.style.display = "none";
        self.commandInput.focus();
    }
    document.getElementById("edit-submit-button").onclick = function() {
        if (self.setCommand.startsWith("set-prop")) {
            self.socket.send(self.setCommand + " " + self.editField.value.replace(/\n/g, "\\n"));
        } else if (self.setCommand.startsWith("exec-script")) {
            self.socket.send(self.setCommand + " " + self.editField.value.replace(/\s+/g, " "));
        } else {
            self.socket.send(self.setCommand + " " + self.editField.value);
        }

        self.editScreen.style.display = "none";
        self.commandInput.focus();
    }
    controller.editField.onkeydown = function(event) {
        if (event.keyCode === keys.KEY_TAB) {
            var value = self.editField.value;
            var start = self.editField.selectionStart;
            var end = self.editField.selectionEnd;

            var content;
            var sizeDiff;
            if (start === end) {
                content = "    ";
            } else {
                content = value.substring(start, end);
                sizeDiff = -content.length;
                if (event.shiftKey) {
                    content = content.replace(/\n    /g, "\n");
                    if (content.substr(0, 4) === "    ") {
                        content = content.substr(4);
                    }
                } else {
                    content = "    " + content.replace(/\n/g, "\n    ");
                    if (content.substr(-5) === "\n    ") {
                        content = content.substr(0, content.length - 4);
                    }
                }
                sizeDiff += content.length;
            }

            self.editField.value = value.substr(0, start) + content + value.substr(end);

            if (start === end) {
                self.editField.selectionStart = start + 4;
                self.editField.selectionEnd = start + 4;
            } else {
                self.editField.selectionStart = start;
                self.editField.selectionEnd = end + sizeDiff;
            }

            return false;
        }
    }

})();
