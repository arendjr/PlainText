(function() {
    
    var onkeypress = controller.commandInput.onkeypress;

    var self = controller;
    controller.commandInput.onkeypress = function(event) {
        if (event.keyCode === keys.KEY_RETURN) {
            var overwriteHistory = false;

            var command = self.commandInput.value;
            if (command.substr(0, 10) === "edit-prop " ||
                command.substr(0, 13) === "edit-trigger ") {

                self.setCommand = "set" + command.substr(4);
                self.commandInput.value = "get" + command.substr(4);

                self.editScreen.style.display = "block";

                self.onMessageHook = function(message) {
                    message = message.trimmed();

                    self.editField.value = message;
                    self.editField.focus();
                }

                overwriteHistory = true;
            } else if (command.substr(0, 1) === "@") {
                self.commandInput.value = "set-prop area " + command.substr(1);

                overwriteHistory = true;
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
        if (self.setCommand.substr(0, 8) === "set-prop") {
            self.socket.send(self.setCommand + " " + self.editField.value.replace(/\n/g, "\\n"));
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
