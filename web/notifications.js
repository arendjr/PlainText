import {
    addCommandListener,
    addIncomingMessageListener,
    removeCommandListener,
} from "./main.js";

let isActive = true;

function attachListeners() {
    window.addEventListener("focus", () => (isActive = true), false);
    window.addEventListener("blur", () => (isActive = false), false);

    function requestPermission() {
        Notification.requestPermission(() =>
            removeCommandListener(requestPermission)
        );
    }

    addCommandListener(requestPermission);

    addIncomingMessageListener(message => {
        if (
            !isActive &&
            Notification.permission === "granted" &&
            /^\w+ (asks|says|tells|shouts)/.test(message)
        ) {
            const notification = new Notification(document.title, {
                body: message,
                onshow: () => setTimeout(() => notification.close(), 10000),
            });
        }
    });
}

attachListeners();
