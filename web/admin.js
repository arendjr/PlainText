function prettyPrint(code) {

    var indent = "";
    var fromIndex = 0;
    do {
        var firstSemicolon = code.indexOf(";", fromIndex);
        var firstOpeningBrace = code.indexOf("{", fromIndex);
        var firstIndentedClosingBrace = code.indexOf("    }", fromIndex);
        var firstClosingBrace = code.indexOf("}", fromIndex);
        var firstOpeningBracket = code.indexOf("(", fromIndex);
        var firstClosingBracket = code.indexOf(")", fromIndex);

        if (firstOpeningBrace < firstSemicolon && firstSemicolon < firstClosingBracket) {
            firstSemicolon = -1;
        }

        if (firstSemicolon === -1) {
            firstSemicolon = 99999;
        }
        if (firstOpeningBrace === -1) {
            firstOpeningBrace = 99999;
        }
        if (firstIndentedClosingBrace === -1) {
            firstIndentedClosingBrace = 99999;
        }
        if (firstClosingBrace === -1) {
            firstClosingBrace = 99999;
        }

        if (firstSemicolon < firstOpeningBrace && firstSemicolon < firstClosingBrace) {
            code = code.substr(0, firstSemicolon + 1) + "\n" +
                   indent + code.substr(firstSemicolon + 1).trimmed();
            fromIndex = firstSemicolon + 1;
        } else if (firstOpeningBrace < firstClosingBrace) {
            indent += "    ";
            code = code.substr(0, firstOpeningBrace + 1) + "\n" +
                   indent + code.substr(firstOpeningBrace + 1).trimmed();
            fromIndex = firstOpeningBrace + 1;
        } else if (firstIndentedClosingBrace < firstClosingBrace) {
            indent = indent.substr(0, indent.length - 4);
            code = code.substr(0, firstIndentedClosingBrace) + "}\n" +
                   indent + code.substr(firstIndentedClosingBrace + 5).trimmed();
            fromIndex = firstIndentedClosingBrace + 2;
        } else {
            indent = indent.substr(0, indent.length - 4);
            code = code.substr(0, firstClosingBrace + 1) + "\n" +
                   indent + code.substr(firstClosingBrace + 1).trimmed();
            fromIndex = firstClosingBrace + 1;
        }
    } while (firstSemicolon < 99999 ||
             firstOpeningBrace < 99999 ||
             firstClosingBrace < 99999);

    code = code.replace(/}\s+(else|while)/g, "} $1");
    code = code.replace(/}\s+(\)|,)/g, "}$1");

    return code;
}

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
                    if (message.substr(0, 9) === "(function" &&
                        message.substr(-2) === "})") {
                        message = prettyPrint(message);
                    }

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
